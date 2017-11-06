/*
Copyright 2017 Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define NOMINMAX
#include <algorithm>
#include <d3d9.h>
#include <dxgi.h>

#include "PresentMonTraceConsumer.hpp"
#include "TraceConsumer.hpp"
#include "DxgkrnlEventStructs.hpp"

using namespace std;

PresentEvent::PresentEvent(EVENT_HEADER const& hdr, ::Runtime runtime)
    : QpcTime(*(uint64_t*) &hdr.TimeStamp)
    , SwapChainAddress(0)
    , SyncInterval(-1)
    , PresentFlags(0)
    , ProcessId(hdr.ProcessId)
    , PresentMode(PresentMode::Unknown)
    , SupportsTearing(false)
    , MMIO(false)
    , SeenDxgkPresent(false)
    , SeenWin32KEvents(false)
    , WasBatched(false)
    , DwmNotified(false)
    , Runtime(runtime)
    , TimeTaken(0)
    , ReadyTime(0)
    , ScreenTime(0)
    , FinalState(PresentResult::Unknown)
    , PlaneIndex(0)
    , QueueSubmitSequence(0)
    , RuntimeThread(hdr.ThreadId)
    , Hwnd(0)
    , TokenPtr(0)
    , Completed(false)
{
}

#ifndef NDEBUG
static bool gPresentMonTraceConsumer_Exiting = false;
#endif

PresentEvent::~PresentEvent()
{
    assert(Completed || gPresentMonTraceConsumer_Exiting);
}

PMTraceConsumer::~PMTraceConsumer()
{
#ifndef NDEBUG
    gPresentMonTraceConsumer_Exiting = true;
#endif
}

void HandleDXGIEvent(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
{
    enum {
        DXGIPresent_Start = 42,
        DXGIPresent_Stop,
        DXGIPresentMPO_Start = 55,
        DXGIPresentMPO_Stop = 56,
    };

    auto const& hdr = pEventRecord->EventHeader;
    switch (hdr.EventDescriptor.Id)
    {
    case DXGIPresent_Start:
    case DXGIPresentMPO_Start:
    {
        PresentEvent event(hdr, Runtime::DXGI);
        GetEventData(pEventRecord, L"pIDXGISwapChain", &event.SwapChainAddress);
        GetEventData(pEventRecord, L"Flags",           &event.PresentFlags);
        GetEventData(pEventRecord, L"SyncInterval",    &event.SyncInterval);

        pmConsumer->RuntimePresentStart(event);
        break;
    }
    case DXGIPresent_Stop:
    case DXGIPresentMPO_Stop:
    {
        auto result = GetEventData<uint32_t>(pEventRecord, L"Result");
        bool AllowBatching = SUCCEEDED(result) && result != DXGI_STATUS_OCCLUDED && result != DXGI_STATUS_MODE_CHANGE_IN_PROGRESS && result != DXGI_STATUS_NO_DESKTOP_ACCESS;
        pmConsumer->RuntimePresentStop(hdr, AllowBatching);
        break;
    }
    }
}

void HandleWin32kEvent(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
{
    enum {
        Win32K_TokenCompositionSurfaceObject = 201,
        Win32K_TokenStateChanged = 301,
    };

    auto const& hdr = pEventRecord->EventHeader;

    uint64_t EventTime = *(uint64_t*)&hdr.TimeStamp;

    switch (hdr.EventDescriptor.Id)
    {
    case Win32K_TokenCompositionSurfaceObject:
    {
        auto eventIter = pmConsumer->FindOrCreatePresent(hdr);

        // Check if we might have retrieved a 'stuck' present from a previous frame.
        if (eventIter->second->SeenWin32KEvents) {
            pmConsumer->mPresentByThreadId.erase(eventIter);
            eventIter = pmConsumer->FindOrCreatePresent(hdr);
        }

        eventIter->second->PresentMode = PresentMode::Composed_Flip;
        eventIter->second->SeenWin32KEvents = true;

        PMTraceConsumer::Win32KPresentHistoryTokenKey key(GetEventData<uint64_t>(pEventRecord, L"CompositionSurfaceLuid"),
            GetEventData<uint64_t>(pEventRecord, L"PresentCount"),
            GetEventData<uint64_t>(pEventRecord, L"BindId"));
        pmConsumer->mWin32KPresentHistoryTokens[key] = eventIter->second;
        break;
    }
    case Win32K_TokenStateChanged:
    {
        PMTraceConsumer::Win32KPresentHistoryTokenKey key(GetEventData<uint64_t>(pEventRecord, L"CompositionSurfaceLuid"),
            GetEventData<uint32_t>(pEventRecord, L"PresentCount"),
            GetEventData<uint64_t>(pEventRecord, L"BindId"));
        auto eventIter = pmConsumer->mWin32KPresentHistoryTokens.find(key);
        if (eventIter == pmConsumer->mWin32KPresentHistoryTokens.end()) {
            return;
        }

        enum class TokenState {
            InFrame = 3,
            Confirmed = 4,
            Retired = 5,
            Discarded = 6,
        };

        auto &event = *eventIter->second;
        auto state = GetEventData<TokenState>(pEventRecord, L"NewState");
        switch (state)
        {
        case TokenState::InFrame:
        {
            // InFrame = composition is starting
            if (event.Hwnd) {
                auto hWndIter = pmConsumer->mPresentByWindow.find(event.Hwnd);
                if (hWndIter == pmConsumer->mPresentByWindow.end()) {
                    pmConsumer->mPresentByWindow.emplace(event.Hwnd, eventIter->second);
                }
                else if (hWndIter->second != eventIter->second) {
                    hWndIter->second->FinalState = PresentResult::Discarded;
                    hWndIter->second = eventIter->second;
                }
            }

            bool iFlip = GetEventData<BOOL>(pEventRecord, L"IndependentFlip") != 0;
            if (iFlip && event.PresentMode == PresentMode::Composed_Flip) {
                event.PresentMode = PresentMode::Hardware_Independent_Flip;
            }

            break;
        }
        case TokenState::Confirmed:
        {
            // Confirmed = present has been submitted
            // If we haven't already decided we're going to discard a token, now's a good time to indicate it'll make it to screen
            if (event.FinalState == PresentResult::Unknown) {
                if (event.PresentFlags & DXGI_PRESENT_DO_NOT_SEQUENCE) {
                    // DO_NOT_SEQUENCE presents may get marked as confirmed,
                    // if a frame was composed when this token was completed
                    event.FinalState = PresentResult::Discarded;
                }
                else {
                    event.FinalState = PresentResult::Presented;
                }
            }
            if (event.Hwnd) {
                pmConsumer->mPresentByWindow.erase(event.Hwnd);
            }
            break;
        }
        case TokenState::Retired:
        {
            // Retired = present has been completed, token's buffer is now displayed
            event.ScreenTime = EventTime;
            break;
        }
        case TokenState::Discarded:
        {
            // Discarded = destroyed - discard if we never got any indication that it was going to screen
            auto sharedPtr = eventIter->second;
            pmConsumer->mWin32KPresentHistoryTokens.erase(eventIter);

            if (event.FinalState == PresentResult::Unknown || event.ScreenTime == 0) {
                event.FinalState = PresentResult::Discarded;
            }

            pmConsumer->CompletePresent(sharedPtr);
            break;
        }
        }
        break;
    }
    }
}

void HandleD3D9Event(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
{
    enum {
        D3D9PresentStart = 1,
        D3D9PresentStop,
    };

    auto const& hdr = pEventRecord->EventHeader;
    switch (hdr.EventDescriptor.Id)
    {
    case D3D9PresentStart:
    {
        PresentEvent event(hdr, Runtime::D3D9);
        GetEventData(pEventRecord, L"pSwapchain", &event.SwapChainAddress);
        uint32_t D3D9Flags = GetEventData<uint32_t>(pEventRecord, L"Flags");
        event.PresentFlags =
            ((D3D9Flags & D3DPRESENT_DONOTFLIP) ? DXGI_PRESENT_DO_NOT_SEQUENCE : 0) |
            ((D3D9Flags & D3DPRESENT_DONOTWAIT) ? DXGI_PRESENT_DO_NOT_WAIT : 0) |
            ((D3D9Flags & D3DPRESENT_FLIPRESTART) ? DXGI_PRESENT_RESTART : 0);
        if ((D3D9Flags & D3DPRESENT_FORCEIMMEDIATE) != 0) {
            event.SyncInterval = 0;
        }

        pmConsumer->RuntimePresentStart(event);
        break;
    }
    case D3D9PresentStop:
    {
        auto result = GetEventData<uint32_t>(pEventRecord, L"Result");
        bool AllowBatching = SUCCEEDED(result) && result != S_PRESENT_OCCLUDED;
        pmConsumer->RuntimePresentStop(hdr, AllowBatching);
        break;
    }
    }
}

void PMTraceConsumer::CompletePresent(std::shared_ptr<PresentEvent> p)
{
    if (p->Completed)
    {
        p->FinalState = PresentResult::Error;
        return;
    }

    // Complete all other presents that were riding along with this one (i.e. this one came from DWM)
    for (auto& p2 : p->DependentPresents) {
        p2->ScreenTime = p->ScreenTime;
        p2->FinalState = PresentResult::Presented;
        CompletePresent(p2);
    }
    p->DependentPresents.clear();

    // Remove it from any tracking maps that it may have been inserted into
    if (p->QueueSubmitSequence != 0) {
        mPresentsBySubmitSequence.erase(p->QueueSubmitSequence);
    }
    if (p->Hwnd != 0) {
        auto hWndIter = mPresentByWindow.find(p->Hwnd);
        if (hWndIter != mPresentByWindow.end() && hWndIter->second == p) {
            mPresentByWindow.erase(hWndIter);
        }
    }
    if (p->TokenPtr != 0) {
        mDxgKrnlPresentHistoryTokens.erase(p->TokenPtr);
    }
    auto& processMap = mPresentsByProcess[p->ProcessId];
    processMap.erase(p->QpcTime);

    auto& presentDeque = mPresentsByProcessAndSwapChain[std::make_tuple(p->ProcessId, p->SwapChainAddress)];
    auto presentIter = presentDeque.begin();
    assert(!presentIter->get()->Completed); // It wouldn't be here anymore if it was

    if (p->FinalState == PresentResult::Presented) {
        while (*presentIter != p) {
            CompletePresent(*presentIter);
            presentIter = presentDeque.begin();
        }
    }

    p->Completed = true;
    if (*presentIter == p) {
        auto lock = scoped_lock(mMutex);
        while (presentIter != presentDeque.end() && presentIter->get()->Completed) {
            mCompletedPresents.push_back(*presentIter);
            presentDeque.pop_front();
            presentIter = presentDeque.begin();
        }
    }

}

decltype(PMTraceConsumer::mPresentByThreadId.begin()) PMTraceConsumer::FindOrCreatePresent(EVENT_HEADER const& hdr)
{
    // Easy: we're on a thread that had some step in the present process
    auto eventIter = mPresentByThreadId.find(hdr.ThreadId);
    if (eventIter != mPresentByThreadId.end()) {
        return eventIter;
    }

    // No such luck, check for batched presents
    auto& processMap = mPresentsByProcess[hdr.ProcessId];
    auto processIter = std::find_if(processMap.begin(), processMap.end(),
        [](auto processIter) {return processIter.second->PresentMode == PresentMode::Unknown; });
    if (processIter == processMap.end()) {
        // This likely didn't originate from a runtime whose events we're tracking (DXGI/D3D9)
        // Could be composition buffers, or maybe another runtime (e.g. GL)
        auto newEvent = std::make_shared<PresentEvent>(hdr, Runtime::Other);
        processMap.emplace(newEvent->QpcTime, newEvent);

        auto& processSwapChainDeque = mPresentsByProcessAndSwapChain[std::make_tuple(hdr.ProcessId, 0ull)];
        processSwapChainDeque.emplace_back(newEvent);

        eventIter = mPresentByThreadId.emplace(hdr.ThreadId, newEvent).first;
    }
    else {
        // Assume batched presents are popped off the front of the driver queue by process in order, do the same here
        eventIter = mPresentByThreadId.emplace(hdr.ThreadId, processIter->second).first;
        processMap.erase(processIter);
    }

    return eventIter;
}

void PMTraceConsumer::RuntimePresentStart(PresentEvent &event)
{
    // Ignore PRESENT_TEST: it's just to check if you're still fullscreen, doesn't actually do anything
    if ((event.PresentFlags & DXGI_PRESENT_TEST) != 0) {
        event.Completed = true;
        return;
    }

    auto pEvent = std::make_shared<PresentEvent>(event);
    mPresentByThreadId[event.RuntimeThread] = pEvent;

    auto& processMap = mPresentsByProcess[event.ProcessId];
    processMap.emplace(event.QpcTime, pEvent);

    auto& processSwapChainDeque = mPresentsByProcessAndSwapChain[std::make_tuple(event.ProcessId, event.SwapChainAddress)];
    processSwapChainDeque.emplace_back(pEvent);

    // Set the caller's local event instance to completed so the assert
    // in ~PresentEvent() doesn't fire when it is destructed.
    event.Completed = true;
}

void PMTraceConsumer::RuntimePresentStop(EVENT_HEADER const& hdr, bool AllowPresentBatching)
{
    auto eventIter = mPresentByThreadId.find(hdr.ThreadId);
    if (eventIter == mPresentByThreadId.end()) {
        return;
    }
    auto &event = *eventIter->second;

    assert(event.QpcTime <= *(uint64_t*) &hdr.TimeStamp);
    event.TimeTaken = *(uint64_t*) &hdr.TimeStamp - event.QpcTime;

    if (!AllowPresentBatching || mSimpleMode) {
        event.FinalState = AllowPresentBatching ? PresentResult::Presented : PresentResult::Discarded;
        CompletePresent(eventIter->second);
    }

    mPresentByThreadId.erase(eventIter);
}

void HandleNTProcessEvent(PEVENT_RECORD pEventRecord, PMTraceConsumer* pmConsumer)
{
    NTProcessEvent event;

    switch (pEventRecord->EventHeader.EventDescriptor.Opcode) {
    case EVENT_TRACE_TYPE_START:
    case EVENT_TRACE_TYPE_DC_START:
        GetEventData(pEventRecord, L"ProcessId",     &event.ProcessId);
        GetEventData(pEventRecord, L"ImageFileName", &event.ImageFileName);
        break;

    case EVENT_TRACE_TYPE_END:
    case EVENT_TRACE_TYPE_DC_END:
        GetEventData(pEventRecord, L"ProcessId", &event.ProcessId);
        break;
    }

    {
        auto lock = scoped_lock(pmConsumer->mNTProcessEventMutex);
        pmConsumer->mNTProcessEvents.emplace_back(event);
    }
}

