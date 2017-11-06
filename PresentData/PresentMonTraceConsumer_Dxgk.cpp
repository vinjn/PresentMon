#define NOMINMAX
#include <algorithm>
#include <dxgi.h>

#include "PresentMonTraceConsumer.hpp"
#include "TraceConsumer.hpp"
#include "DxgkrnlEventStructs.hpp"
#include "DxgkEventEnum.h"

using namespace std;


void PMTraceConsumer::HandleDxgkBlt(DxgkBltEventArgs& args)
{
    auto eventIter = FindOrCreatePresent(*args.pEventHeader);

    // Check if we might have retrieved a 'stuck' present from a previous frame.
    // If the present mode isn't unknown at this point, we've already seen this present progress further
    if (eventIter->second->PresentMode != PresentMode::Unknown) {
        mPresentByThreadId.erase(eventIter);
        eventIter = FindOrCreatePresent(*args.pEventHeader);
    }

    // This could be one of several types of presents. Further events will clarify.
    // For now, assume that this is a blt straight into a surface which is already on-screen.
    eventIter->second->PresentMode = args.Present ?
        PresentMode::Composed_Copy_CPU_GDI : PresentMode::Hardware_Legacy_Copy_To_Front_Buffer;
    eventIter->second->SupportsTearing = !args.Present;
    eventIter->second->Hwnd = args.Hwnd;
}

void PMTraceConsumer::HandleDxgkFlip(DxgkFlipEventArgs& args)
{
    // A flip event is emitted during fullscreen present submission.
    // Afterwards, expect an MMIOFlip packet on the same thread, used
    // to trace the flip to screen.
    auto eventIter = FindOrCreatePresent(*args.pEventHeader);

    // Check if we might have retrieved a 'stuck' present from a previous frame.
    // The only events that we can expect before a Flip/FlipMPO are a runtime present start, or a previous FlipMPO.
    if (eventIter->second->QueueSubmitSequence != 0 || eventIter->second->SeenDxgkPresent) {
        // It's already progressed further but didn't complete, ignore it and create a new one.
        mPresentByThreadId.erase(eventIter);
        eventIter = FindOrCreatePresent(*args.pEventHeader);
    }

    if (eventIter->second->PresentMode != PresentMode::Unknown) {
        // For MPO, N events may be issued, but we only care about the first
        return;
    }

    eventIter->second->MMIO = args.MMIO;
    eventIter->second->PresentMode = PresentMode::Hardware_Legacy_Flip;

    if (eventIter->second->SyncInterval == -1) {
        eventIter->second->SyncInterval = args.FlipInterval;
    }
    if (!args.MMIO) {
        eventIter->second->SupportsTearing = args.FlipInterval == 0;
    }

    // If this is the DWM thread, piggyback these pending presents on our fullscreen present
    if (args.pEventHeader->ThreadId == DwmPresentThreadId) {
        std::swap(eventIter->second->DependentPresents, mPresentsWaitingForDWM);
        DwmPresentThreadId = 0;
    }
}

void PMTraceConsumer::HandleDxgkQueueSubmit(DxgkQueueSubmitEventArgs& args)
{
    // If we know we're never going to get a DxgkPresent event for a given blt, then let's try to determine if it's a redirected blt or not.
    // If it's redirected, then the SubmitPresentHistory event should've been emitted before submitting anything else to the same context,
    // and therefore we'll know it's a redirected present by this point. If it's still non-redirected, then treat this as if it was a DxgkPresent
    // event - the present will be considered completed once its work is done, or if the work is already done, complete it now.
    if (!args.SupportsDxgkPresentEvent) {
        auto eventIter = mBltsByDxgContext.find(args.Context);
        if (eventIter != mBltsByDxgContext.end()) {
            if (eventIter->second->PresentMode == PresentMode::Hardware_Legacy_Copy_To_Front_Buffer) {
                eventIter->second->SeenDxgkPresent = true;
                if (eventIter->second->ScreenTime != 0) {
                    CompletePresent(eventIter->second);
                }
            }
            mBltsByDxgContext.erase(eventIter);
        }
    }

    // This event is emitted after a flip/blt/PHT event, and may be the only way
    // to trace completion of the present.
    if (args.PacketType == DxgKrnl_QueueSubmit_Type::MMIOFlip ||
        args.PacketType == DxgKrnl_QueueSubmit_Type::Software ||
        args.Present) {
        auto eventIter = mPresentByThreadId.find(args.pEventHeader->ThreadId);
        if (eventIter == mPresentByThreadId.end() || eventIter->second->QueueSubmitSequence != 0) {
            return;
        }

        eventIter->second->QueueSubmitSequence = args.SubmitSequence;
        mPresentsBySubmitSequence.emplace(args.SubmitSequence, eventIter->second);

        if (eventIter->second->PresentMode == PresentMode::Hardware_Legacy_Copy_To_Front_Buffer && !args.SupportsDxgkPresentEvent) {
            mBltsByDxgContext[args.Context] = eventIter->second;
        }
    }
}

void PMTraceConsumer::HandleDxgkQueueComplete(DxgkQueueCompleteEventArgs& args)
{
    auto eventIter = mPresentsBySubmitSequence.find(args.SubmitSequence);
    if (eventIter == mPresentsBySubmitSequence.end()) {
        return;
    }

    auto pEvent = eventIter->second;

    uint64_t EventTime = *(uint64_t*)&args.pEventHeader->TimeStamp;

    if (pEvent->PresentMode == PresentMode::Hardware_Legacy_Copy_To_Front_Buffer ||
        (pEvent->PresentMode == PresentMode::Hardware_Legacy_Flip && !pEvent->MMIO)) {
        pEvent->ScreenTime = pEvent->ReadyTime = EventTime;
        pEvent->FinalState = PresentResult::Presented;

        // Sometimes, the queue packets associated with a present will complete before the DxgKrnl present event is fired
        // In this case, for blit presents, we have no way to differentiate between fullscreen and windowed blits
        // So, defer the completion of this present until we know all events have been fired
        if (pEvent->SeenDxgkPresent || pEvent->PresentMode != PresentMode::Hardware_Legacy_Copy_To_Front_Buffer) {
            CompletePresent(pEvent);
        }
    }
}

void PMTraceConsumer::HandleDxgkMMIOFlip(DxgkMMIOFlipEventArgs& args)
{
    // An MMIOFlip event is emitted when an MMIOFlip packet is dequeued.
    // This corresponds to all GPU work prior to the flip being completed
    // (i.e. present "ready")
    // It also is emitted when an independent flip PHT is dequed,
    // and will tell us whether the present is immediate or vsync.
    auto eventIter = mPresentsBySubmitSequence.find(args.FlipSubmitSequence);
    if (eventIter == mPresentsBySubmitSequence.end()) {
        return;
    }

    uint64_t EventTime = *(uint64_t*)&args.pEventHeader->TimeStamp;
    eventIter->second->ReadyTime = EventTime;

    if (eventIter->second->PresentMode == PresentMode::Composed_Flip) {
        eventIter->second->PresentMode = PresentMode::Hardware_Independent_Flip;
    }

    if (args.Flags & DxgKrnl_MMIOFlip_Flags::FlipImmediate) {
        eventIter->second->FinalState = PresentResult::Presented;
        eventIter->second->ScreenTime = EventTime;
        eventIter->second->SupportsTearing = true;
        if (eventIter->second->PresentMode == PresentMode::Hardware_Legacy_Flip) {
            CompletePresent(eventIter->second);
        }
    }
}

void PMTraceConsumer::HandleDxgkVSyncDPC(DxgkVSyncDPCEventArgs& args)
{
    // The VSyncDPC contains a field telling us what flipped to screen.
    // This is the way to track completion of a fullscreen present.
    auto eventIter = mPresentsBySubmitSequence.find(args.FlipSubmitSequence);
    if (eventIter == mPresentsBySubmitSequence.end()) {
        return;
    }

    uint64_t EventTime = *(uint64_t*)&args.pEventHeader->TimeStamp;
    eventIter->second->ScreenTime = EventTime;
    eventIter->second->FinalState = PresentResult::Presented;
    if (eventIter->second->PresentMode == PresentMode::Hardware_Legacy_Flip) {
        CompletePresent(eventIter->second);
    }
}

void PMTraceConsumer::HandleDxgkSubmitPresentHistoryEventArgs(DxgkSubmitPresentHistoryEventArgs& args)
{
    // These events are emitted during submission of all types of windowed presents while DWM is on.
    // It gives us up to two different types of keys to correlate further.
    auto eventIter = FindOrCreatePresent(*args.pEventHeader);

    // Check if we might have retrieved a 'stuck' present from a previous frame.
    if (eventIter->second->TokenPtr != 0) {
        // It's already progressed further but didn't complete, ignore it and create a new one.
        mPresentByThreadId.erase(eventIter);
        eventIter = FindOrCreatePresent(*args.pEventHeader);
    }

    eventIter->second->ReadyTime = eventIter->second->ScreenTime = 0;
    eventIter->second->SupportsTearing = false;
    eventIter->second->FinalState = PresentResult::Unknown;
    eventIter->second->TokenPtr = args.Token;

    if (eventIter->second->PresentMode == PresentMode::Hardware_Legacy_Copy_To_Front_Buffer)
    {
        eventIter->second->PresentMode = PresentMode::Composed_Copy_GPU_GDI;
        assert(args.KnownPresentMode == PresentMode::Unknown ||
            args.KnownPresentMode == PresentMode::Composed_Copy_GPU_GDI);
    }
    else if (eventIter->second->PresentMode == PresentMode::Unknown)
    {
        if (args.KnownPresentMode == PresentMode::Composed_Composition_Atlas) {
            eventIter->second->PresentMode = PresentMode::Composed_Composition_Atlas;
        }
        else {
            // When there's no Win32K events, we'll assume PHTs that aren't after a blt, and aren't composition tokens
            // are flip tokens and that they're displayed. There are no Win32K events on Win7, and they might not be
            // present in some traces - don't let presents get stuck/dropped just because we can't track them perfectly.
            assert(!eventIter->second->SeenWin32KEvents);
            eventIter->second->PresentMode = PresentMode::Composed_Flip;
        }
    }
    else if (eventIter->second->PresentMode == PresentMode::Composed_Copy_CPU_GDI) {
        if (args.TokenData == 0) {
            // This is the best we can do, we won't be able to tell how many frames are actually displayed.
            mPresentsWaitingForDWM.emplace_back(eventIter->second);
        }
        else {
            mPresentsByLegacyBlitToken[args.TokenData] = eventIter->second;
        }
    }
    mDxgKrnlPresentHistoryTokens[args.Token] = eventIter->second;
}

void PMTraceConsumer::HandleDxgkPropagatePresentHistoryEventArgs(DxgkPropagatePresentHistoryEventArgs& args)
{
    // This event is emitted when a token is being handed off to DWM, and is a good way to indicate a ready state
    auto eventIter = mDxgKrnlPresentHistoryTokens.find(args.Token);
    if (eventIter == mDxgKrnlPresentHistoryTokens.end()) {
        return;
    }

    uint64_t EventTime = *(uint64_t*)&args.pEventHeader->TimeStamp;
    auto& ReadyTime = eventIter->second->ReadyTime;
    ReadyTime = (ReadyTime == 0 ?
        EventTime : std::min(ReadyTime, EventTime));

    if (eventIter->second->PresentMode == PresentMode::Composed_Composition_Atlas ||
        (eventIter->second->PresentMode == PresentMode::Composed_Flip && !eventIter->second->SeenWin32KEvents)) {
        mPresentsWaitingForDWM.emplace_back(eventIter->second);
    }

    if (eventIter->second->PresentMode == PresentMode::Composed_Copy_GPU_GDI) {
        // Manipulate the map here
        // When DWM is ready to present, we'll query for the most recent blt targeting this window and take it out of the map
        mPresentByWindow[eventIter->second->Hwnd] = eventIter->second;
    }

    mDxgKrnlPresentHistoryTokens.erase(eventIter);
}

static PresentMode D3DKMT_TokenModel_ToPresentMode(D3DKMT_PRESENT_MODEL Model)
{
    switch (Model)
    {
    case D3DKMT_PM_REDIRECTED_BLT:
        return PresentMode::Composed_Copy_GPU_GDI;
    case D3DKMT_PM_REDIRECTED_VISTABLT:
        return PresentMode::Composed_Copy_CPU_GDI;
    case D3DKMT_PM_REDIRECTED_FLIP:
        return PresentMode::Composed_Flip;
    case D3DKMT_PM_REDIRECTED_COMPOSITION:
        return PresentMode::Composed_Composition_Atlas;
    }
    return PresentMode::Unknown;
}

void HandleDXGKEvent(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
{
    auto const& hdr = pEventRecord->EventHeader;
    bool result = true;

    uint64_t EventTime = *(uint64_t*)&hdr.TimeStamp;
    auto id = hdr.EventDescriptor.Id;
    switch (id)
    {
    case dxgk::HistoryBuffer:
    {
        DxgkHistoryBufferArgs args = {};
        args.pEventHeader = &hdr;
        result = GetEventData(pEventRecord, L"hContext", &args.hContext);
        result = GetEventData(pEventRecord, L"RenderCbSequence", &args.RenderCbSequence);
        result = GetEventData(pEventRecord, L"DmaSubmissionSequence", &args.DmaSubmissionSequence);
        result = GetEventData(pEventRecord, L"Precision", &args.Precision);
        result = GetEventData(pEventRecord, L"HistoryBufferSize", &args.HistoryBufferSize);
        args.HistoryBuffer.resize(args.HistoryBufferSize / 8);
        result = GetEventDataEx(pEventRecord, L"HistoryBuffer", args.HistoryBuffer.data(), args.HistoryBufferSize);

        break;
    }
    case dxgk::ContextStart:
    case dxgk::ContextStop:
    {
        ContextStartArgs args = {};
        args.pEventHeader = &hdr;
        result = GetEventData(pEventRecord, L"hDevice", &args.hDevice);
        result = GetEventData(pEventRecord, L"NodeOrdinal", &args.NodeOrdinal);
        result = GetEventData(pEventRecord, L"EngineAffinity", &args.EngineAffinity);
        result = GetEventData(pEventRecord, L"DmaBufferSize", &args.DmaBufferSize);
        result = GetEventData(pEventRecord, L"DmaBufferSegmentSet", &args.DmaBufferSegmentSet);
        result = GetEventData(pEventRecord, L"DmaBufferPrivateDataSize", &args.DmaBufferPrivateDataSize);
        result = GetEventData(pEventRecord, L"AllocationListSize", &args.AllocationListSize);
        result = GetEventData(pEventRecord, L"PatchLocationListSize", &args.PatchLocationListSize);
        result = GetEventData(pEventRecord, L"Flags", &args.Flags);
        result = GetEventData(pEventRecord, L"hContext", &args.hContext);
        result = GetEventData(pEventRecord, L"ContextHandle", &args.ContextHandle);
        result = GetEventData(pEventRecord, L"ParentDxgContext", &args.ParentDxgContext);

        args.isStart = (id == dxgk::ContextStart);
        break;
    }
    case dxgk::Flip:
    case dxgk::FlipMultiPlaneOverlay:
    {
        DxgkFlipEventArgs args = {};
        args.pEventHeader = &hdr;
        args.FlipInterval = -1;
        if (hdr.EventDescriptor.Id == dxgk::Flip) {
            args.FlipInterval = GetEventData<uint32_t>(pEventRecord, L"FlipInterval");
            args.MMIO = GetEventData<BOOL>(pEventRecord, L"MMIOFlip") != 0;
        }
        else {
            args.MMIO = true; // All MPO flips are MMIO
        }
        pmConsumer->HandleDxgkFlip(args);
        break;
    }
    case dxgk::QueuePacketStart:
    {
        DxgkQueueSubmitEventArgs args = {};
        args.pEventHeader = &hdr;
        args.PacketType = GetEventData<DxgKrnl_QueueSubmit_Type>(pEventRecord, L"PacketType");
        args.SubmitSequence = GetEventData<uint32_t>(pEventRecord, L"SubmitSequence");
        args.Present = GetEventData<BOOL>(pEventRecord, L"bPresent") != 0;
        args.Context = GetEventData<uint64_t>(pEventRecord, L"hContext");
        args.SupportsDxgkPresentEvent = true;
        pmConsumer->HandleDxgkQueueSubmit(args);
        break;
    }
    case dxgk::QueuePacketStop:
    {
        DxgkQueueCompleteEventArgs args = {};
        args.pEventHeader = &hdr;
        args.SubmitSequence = GetEventData<uint32_t>(pEventRecord, L"SubmitSequence");
        pmConsumer->HandleDxgkQueueComplete(args);
        break;
    }
    case dxgk::MMIOFlip:
    {
        DxgkMMIOFlipEventArgs args = {};
        args.pEventHeader = &hdr;
        args.FlipSubmitSequence = GetEventData<uint32_t>(pEventRecord, L"FlipSubmitSequence");
        args.Flags = GetEventData<DxgKrnl_MMIOFlip_Flags>(pEventRecord, L"Flags");
        pmConsumer->HandleDxgkMMIOFlip(args);
        break;
    }
    case dxgk::MMIOFlipMultiPlaneOverlay:
    {
        // See above for more info about this packet.
        // Note: Event does not exist on Win7
        auto FlipFenceId = GetEventData<uint64_t>(pEventRecord, L"FlipSubmitSequence");
        uint32_t FlipSubmitSequence = (uint32_t)(FlipFenceId >> 32u);

        auto eventIter = pmConsumer->mPresentsBySubmitSequence.find(FlipSubmitSequence);
        if (eventIter == pmConsumer->mPresentsBySubmitSequence.end()) {
            return;
        }

        // Avoid double-marking a single present packet coming from the MPO API
        if (eventIter->second->ReadyTime == 0) {
            eventIter->second->ReadyTime = EventTime;
            eventIter->second->PlaneIndex = GetEventData<uint32_t>(pEventRecord, L"LayerIndex");
        }

        if (eventIter->second->PresentMode == PresentMode::Hardware_Independent_Flip ||
            eventIter->second->PresentMode == PresentMode::Composed_Flip) {
            eventIter->second->PresentMode = PresentMode::Hardware_Composed_Independent_Flip;
        }

        if (hdr.EventDescriptor.Version >= 2)
        {
            enum class DxgKrnl_MMIOFlipMPO_FlipEntryStatus {
                FlipWaitVSync = 5,
                FlipWaitComplete = 11,
                // There are others, but they're more complicated to deal with.
            };

            auto FlipEntryStatusAfterFlip = GetEventData<DxgKrnl_MMIOFlipMPO_FlipEntryStatus>(pEventRecord, L"FlipEntryStatusAfterFlip");
            if (FlipEntryStatusAfterFlip != DxgKrnl_MMIOFlipMPO_FlipEntryStatus::FlipWaitVSync) {
                eventIter->second->FinalState = PresentResult::Presented;
                eventIter->second->SupportsTearing = true;
                if (FlipEntryStatusAfterFlip == DxgKrnl_MMIOFlipMPO_FlipEntryStatus::FlipWaitComplete) {
                    eventIter->second->ScreenTime = EventTime;
                }
                if (eventIter->second->PresentMode == PresentMode::Hardware_Legacy_Flip) {
                    pmConsumer->CompletePresent(eventIter->second);
                }
            }
        }

        break;
    }
    case dxgk::VSyncDPC:
    {
        auto FlipFenceId = GetEventData<uint64_t>(pEventRecord, L"FlipFenceId");

        DxgkVSyncDPCEventArgs args = {};
        args.pEventHeader = &hdr;
        args.FlipSubmitSequence = (uint32_t)(FlipFenceId >> 32u);
        pmConsumer->HandleDxgkVSyncDPC(args);
        break;
    }
    case dxgk::Present:
    {
        // This event is emitted at the end of the kernel present, before returning.
        // The presence of this event is used with blt presents to indicate that no
        // PHT is to be expected.
        auto eventIter = pmConsumer->mPresentByThreadId.find(hdr.ThreadId);
        if (eventIter == pmConsumer->mPresentByThreadId.end()) {
            return;
        }

        eventIter->second->SeenDxgkPresent = true;
        if (eventIter->second->Hwnd == 0) {
            eventIter->second->Hwnd = GetEventData<uint64_t>(pEventRecord, L"hWindow");
        }

        if (eventIter->second->PresentMode == PresentMode::Hardware_Legacy_Copy_To_Front_Buffer &&
            eventIter->second->ScreenTime != 0) {
            // This is a fullscreen or DWM-off blit where all work associated was already done, so it's on-screen
            // It was deferred to here because there was no way to be sure it was really fullscreen until now
            pmConsumer->CompletePresent(eventIter->second);
        }

        if (eventIter->second->RuntimeThread != hdr.ThreadId) {
            if (eventIter->second->TimeTaken == 0) {
                eventIter->second->TimeTaken = EventTime - eventIter->second->QpcTime;
            }
            eventIter->second->WasBatched = true;
            pmConsumer->mPresentByThreadId.erase(eventIter);
        }
        break;
    }
    case dxgk::PresentHistoryDetailedStart:
    case dxgk::PresentHistoryStart:
    {
        DxgkSubmitPresentHistoryEventArgs args = {};
        args.pEventHeader = &hdr;
        args.Token = GetEventData<uint64_t>(pEventRecord, L"Token");
        args.TokenData = GetEventData<uint64_t>(pEventRecord, L"TokenData");
        auto KMTPresentModel = GetEventData<D3DKMT_PRESENT_MODEL>(pEventRecord, L"Model");
        args.KnownPresentMode = D3DKMT_TokenModel_ToPresentMode(KMTPresentModel);
        if (KMTPresentModel != D3DKMT_PM_REDIRECTED_GDI)
        {
            pmConsumer->HandleDxgkSubmitPresentHistoryEventArgs(args);
        }
        break;
    }
    case dxgk::PresentHistory:
    {
        DxgkPropagatePresentHistoryEventArgs args = {};
        args.pEventHeader = &hdr;
        args.Token = GetEventData<uint64_t>(pEventRecord, L"Token");
        pmConsumer->HandleDxgkPropagatePresentHistoryEventArgs(args);
        break;
    }
    case dxgk::Blit:
    {
        DxgkBltEventArgs args = {};
        args.pEventHeader = &hdr;
        args.Hwnd = GetEventData<uint64_t>(pEventRecord, L"hwnd");
        args.Present = GetEventData<uint32_t>(pEventRecord, L"bRedirectedPresent") != 0;
        pmConsumer->HandleDxgkBlt(args);
        break;
    }
    }
}
