#define NOMINMAX
#include <algorithm>
#include <dxgi.h>

#include "PresentMonTraceConsumer.hpp"
#include "TraceConsumer.hpp"
#include "DxgkrnlEventStructs.hpp"

using namespace std;

namespace Win7
{
    void HandleDxgkBlt(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
    {
        DxgkBltEventArgs Args = {};
        Args.pEventHeader = &pEventRecord->EventHeader;
        auto pBltEvent = reinterpret_cast<DXGKETW_BLTEVENT*>(pEventRecord->UserData);
        Args.Hwnd = pBltEvent->hwnd;
        Args.Present = pBltEvent->bRedirectedPresent != 0;
        pmConsumer->HandleDxgkBlt(Args);
    }

    void HandleDxgkFlip(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
    {
        DxgkFlipEventArgs Args = {};
        Args.pEventHeader = &pEventRecord->EventHeader;
        auto pFlipEvent = reinterpret_cast<DXGKETW_FLIPEVENT*>(pEventRecord->UserData);
        Args.FlipInterval = pFlipEvent->FlipInterval;
        Args.MMIO = pFlipEvent->MMIOFlip != 0;
        pmConsumer->HandleDxgkFlip(Args);
    }

    void HandleDxgkPresentHistory(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
    {
        auto pPresentHistoryEvent = reinterpret_cast<DXGKETW_PRESENTHISTORYEVENT*>(pEventRecord->UserData);
        if (pEventRecord->EventHeader.EventDescriptor.Opcode == EVENT_TRACE_TYPE_START)
        {
            DxgkSubmitPresentHistoryEventArgs Args = {};
            Args.pEventHeader = &pEventRecord->EventHeader;
            Args.KnownPresentMode = PresentMode::Unknown;
            Args.Token = pPresentHistoryEvent->Token;
            pmConsumer->HandleDxgkSubmitPresentHistoryEventArgs(Args);
        }
        else if (pEventRecord->EventHeader.EventDescriptor.Opcode == EVENT_TRACE_TYPE_INFO)
        {
            DxgkPropagatePresentHistoryEventArgs Args = {};
            Args.pEventHeader = &pEventRecord->EventHeader;
            Args.Token = pPresentHistoryEvent->Token;
            pmConsumer->HandleDxgkPropagatePresentHistoryEventArgs(Args);
        }
    }

    void HandleDxgkQueuePacket(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
    {
        if (pEventRecord->EventHeader.EventDescriptor.Opcode == EVENT_TRACE_TYPE_START)
        {
            DxgkQueueSubmitEventArgs Args = {};
            Args.pEventHeader = &pEventRecord->EventHeader;
            auto pSubmitEvent = reinterpret_cast<DXGKETW_QUEUESUBMITEVENT*>(pEventRecord->UserData);
            switch (pSubmitEvent->PacketType)
            {
            case DXGKETW_MMIOFLIP_COMMAND_BUFFER:
                Args.PacketType = DxgKrnl_QueueSubmit_Type::MMIOFlip;
                break;
            case DXGKETW_SOFTWARE_COMMAND_BUFFER:
                Args.PacketType = DxgKrnl_QueueSubmit_Type::Software;
                break;
            default:
                Args.Present = pSubmitEvent->bPresent != 0;
                break;
            }
            Args.SubmitSequence = pSubmitEvent->SubmitSequence;
            Args.Context = pSubmitEvent->hContext;
            pmConsumer->HandleDxgkQueueSubmit(Args);
        }
        else if (pEventRecord->EventHeader.EventDescriptor.Opcode == EVENT_TRACE_TYPE_STOP)
        {
            DxgkQueueCompleteEventArgs Args = {};
            Args.pEventHeader = &pEventRecord->EventHeader;
            auto pCompleteEvent = reinterpret_cast<DXGKETW_QUEUECOMPLETEEVENT*>(pEventRecord->UserData);
            Args.SubmitSequence = pCompleteEvent->SubmitSequence;
            pmConsumer->HandleDxgkQueueComplete(Args);
        }
    }

    void HandleDxgkVSyncDPC(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
    {
        DxgkVSyncDPCEventArgs Args = {};
        Args.pEventHeader = &pEventRecord->EventHeader;
        auto pVSyncDPCEvent = reinterpret_cast<DXGKETW_SCHEDULER_VSYNC_DPC*>(pEventRecord->UserData);
        Args.FlipSubmitSequence = (uint32_t)(pVSyncDPCEvent->FlipFenceId.QuadPart >> 32u);
        pmConsumer->HandleDxgkVSyncDPC(Args);
    }

    void HandleDxgkMMIOFlip(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
    {
        DxgkMMIOFlipEventArgs Args = {};
        Args.pEventHeader = &pEventRecord->EventHeader;
        if (pEventRecord->EventHeader.Flags & EVENT_HEADER_FLAG_32_BIT_HEADER)
        {
            auto pMMIOFlipEvent = reinterpret_cast<DXGKETW_SCHEDULER_MMIO_FLIP_32*>(pEventRecord->UserData);
            Args.Flags = static_cast<DxgKrnl_MMIOFlip_Flags>(pMMIOFlipEvent->Flags);
            Args.FlipSubmitSequence = pMMIOFlipEvent->FlipSubmitSequence;
        }
        else
        {
            auto pMMIOFlipEvent = reinterpret_cast<DXGKETW_SCHEDULER_MMIO_FLIP_64*>(pEventRecord->UserData);
            Args.Flags = static_cast<DxgKrnl_MMIOFlip_Flags>(pMMIOFlipEvent->Flags);
            Args.FlipSubmitSequence = pMMIOFlipEvent->FlipSubmitSequence;
        }
        pmConsumer->HandleDxgkMMIOFlip(Args);
    }
}
