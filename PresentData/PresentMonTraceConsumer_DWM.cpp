#define NOMINMAX
#include <algorithm>
#include <dxgi.h>

#include "PresentMonTraceConsumer.hpp"
#include "TraceConsumer.hpp"

using namespace std;

void HandleDWMEvent(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
{
    enum {
        DWM_GetPresentHistory = 64,
        DWM_Schedule_Present_Start = 15,
        DWM_FlipChain_Pending = 69,
        DWM_FlipChain_Complete = 70,
        DWM_FlipChain_Dirty = 101,
        DWM_Schedule_SurfaceUpdate = 196,
    };

    auto& hdr = pEventRecord->EventHeader;
    switch (hdr.EventDescriptor.Id)
    {
    case DWM_GetPresentHistory:
    {
        for (auto& hWndPair : pmConsumer->mPresentByWindow)
        {
            auto& present = hWndPair.second;
            // Pickup the most recent present from a given window
            if (present->PresentMode != PresentMode::Composed_Copy_GPU_GDI &&
                present->PresentMode != PresentMode::Composed_Copy_CPU_GDI) {
                continue;
            }
            present->DwmNotified = true;
            pmConsumer->mPresentsWaitingForDWM.emplace_back(present);
        }
        pmConsumer->mPresentByWindow.clear();
        break;
    }
    case DWM_Schedule_Present_Start:
    {
        pmConsumer->DwmPresentThreadId = hdr.ThreadId;
        break;
    }
    case DWM_FlipChain_Pending:
    case DWM_FlipChain_Complete:
    case DWM_FlipChain_Dirty:
    {
        if (InlineIsEqualGUID(hdr.ProviderId, Win7::DWM_PROVIDER_GUID)) {
            return;
        }
        // As it turns out, the 64-bit token data from the PHT submission is actually two 32-bit data chunks,		
        // corresponding to a "flip chain" id and present id		
        uint32_t flipChainId = (uint32_t)GetEventData<uint64_t>(pEventRecord, L"ulFlipChain");
        uint32_t serialNumber = (uint32_t)GetEventData<uint64_t>(pEventRecord, L"ulSerialNumber");
        uint64_t token = ((uint64_t)flipChainId << 32ull) | serialNumber;
        auto flipIter = pmConsumer->mDxgKrnlPresentHistoryTokens.find(token);
        if (flipIter == pmConsumer->mDxgKrnlPresentHistoryTokens.end()) {
            return;
        }

        // Watch for multiple legacy blits completing against the same window		
        auto hWnd = GetEventData<uint64_t>(pEventRecord, L"hwnd");
        pmConsumer->mPresentByWindow[hWnd] = flipIter->second;
        flipIter->second->DwmNotified = true;
        pmConsumer->mPresentsByLegacyBlitToken.erase(flipIter);
        break;
    }
    case DWM_Schedule_SurfaceUpdate:
    {
        PMTraceConsumer::Win32KPresentHistoryTokenKey key(GetEventData<uint64_t>(pEventRecord, L"luidSurface"),
            GetEventData<uint64_t>(pEventRecord, L"PresentCount"),
            GetEventData<uint64_t>(pEventRecord, L"bindId"));
        auto eventIter = pmConsumer->mWin32KPresentHistoryTokens.find(key);
        if (eventIter != pmConsumer->mWin32KPresentHistoryTokens.end()) {
            eventIter->second->DwmNotified = true;
        }
        break;
    }
    }
}
