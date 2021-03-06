#define NOMINMAX
#include <algorithm>
#include <d3d12.h>
#include <dxgi.h>

#include "PresentMonTraceConsumer.hpp"
#include "TraceConsumer.hpp"
#include "D3d12EventStructs.hpp"

using namespace std;

enum D3d12Evt
{
    Name = 1,
    NameDCStart = 2,
    DeviceStart = 3,
    DeviceStop = 4,
    DeviceDCStart = 5,
    Device = 19,
    D3DSupplemental = 26,
    Marker = 35,
    MarkerStart = 36,
    MarkerStop = 37,
    Marker38 = 38,
    RuntimeMarker = 39,
    RuntimeDroppedCall = 40,
    RuntimeMarker41 = 41,
    CustomDriverMarker = 45,
    CommandQueueStart = 47,
    CommandQueueStop = 48,
    CommandQueueDCStart = 49,
    CommandListStart = 50,
    CommandListStop = 51,
    CommandList = 52,
    ExecuteCommandListStart = 53,
    ExecuteCommandListStop = 54,
    Name55 = 55,
    NameDCStart56 = 56,
    FenceStart = 57,
    FenceStop = 58,
    FenceDCStart = 59,
    CommandAllocatorStart = 60,
    CommandAllocatorStop = 61,
    CommandAllocatorDCStart = 62,
    GraphicsPipelineStateStart = 63,
    GraphicsPipelineStateStop = 64,
    GraphicsPipelineStateDCStart = 65,
    DescriptorHeapStart = 66,
    DescriptorHeapStop = 67,
    DescriptorHeapDCStart = 68,
    RootSignatureStart = 69,
    RootSignatureStop = 70,
    RootSignatureDCStart = 71,
    HeapStart = 72,
    HeapStop = 73,
    HeapDCStart = 74,
    QueryHeapStart = 75,
    QueryHeapStop = 76,
    QueryHeapDCStart = 77,
    CommandSignatureStart = 78,
    CommandSignatureStop = 79,
    CommandSignatureDCStart = 80,
    PipelineLibraryStart = 81,
    PipelineLibraryStop = 82,
    PipelineLibraryDCStart = 83,
    VideoDecoderStart = 84,
    VideoDecoderStop = 85,
    VideoDecoderDCStart = 86,
    VideoDecodeStreamStart = 87,
    VideoDecodeStreamStop = 88,
    VideoDecodeStreamDCStart = 89,
    CommandListDCStart = 90,
    ResourceStart = 91,
    ResourceStop = 92,
    ResourceDCStart = 93,
    AllocationInfo_V1 = 94,
    CommandAllocatorUnacquire = 95,
};

void HandleD3D12Event(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
{
    auto const& hdr = pEventRecord->EventHeader;
    //fprintf(stderr, "Id: %d, Opcode: %d\n", hdr.EventDescriptor.Id, hdr.EventDescriptor.Opcode);
    bool result = true;
    auto id = hdr.EventDescriptor.Id;
    switch (id)
    {
    case CommandListStart:
    case CommandListStop:
    {
        d3d12event::CommandListStartArgs args = {};
        result = GetEventData(pEventRecord, L"pID3D12Device", &args.pID3D12Device);
        result = GetEventData(pEventRecord, L"pID3D12CommandList", &args.pID3D12CommandList);
        result = GetEventData(pEventRecord, L"SequenceNumber", &args.SequenceNumber);
        result = GetEventData(pEventRecord, L"type", &args.type);
        args.isStart = (id == CommandListStart);
        break;
    }
    case Name:
    {
        d3d12event::NameArgs args = {};
        uint32_t CchOldDebugObjectName;
        uint32_t CchNewDebugObjectName;
        result = GetEventData(pEventRecord, L"pObject", &args.pObject);
        result = GetEventData(pEventRecord, L"CchOldDebugObjectName", &CchOldDebugObjectName);
        if (CchOldDebugObjectName > 0)
        {
            result = GetEventData(pEventRecord, L"OldDebugObjectName", &args.OldDebugObjectName);
        }
        result = GetEventData(pEventRecord, L"CchNewDebugObjectName", &CchNewDebugObjectName);
        result = GetEventData(pEventRecord, L"NewDebugObjectName", &args.NewDebugObjectName);
        break;
    }
#if 0
    case RuntimeMarker:
    {
        d3d12event::RuntimeMarkerArgs args = {};
        result = GetEventData(pEventRecord, L"CPUFrequency", &args.CPUFrequency);
        result = GetEventData(pEventRecord, L"FirstAPISequenceNumber", &args.FirstAPISequenceNumber);
        result = GetEventData(pEventRecord, L"pID3D12CommandList", &args.pID3D12CommandList);
        result = GetEventData(pEventRecord, L"CPUTimeHigh", &args.CPUTimeHigh);
        result = GetEventData(pEventRecord, L"ThreadIDCount", &args.ThreadIDCount);
        args.ThreadIDs.resize(args.ThreadIDCount);
        result = GetEventDataEx(pEventRecord, L"ThreadIDs", args.ThreadIDs.data(), args.ThreadIDCount * sizeof(args.ThreadIDs[0]));
        result = GetEventData(pEventRecord, L"DataSize", &args.DataSize);
        args.Data.resize(args.DataSize);
        result = GetEventDataEx(pEventRecord, L"Data", args.Data.data(), args.DataSize);
        break;
    }
#endif
    case RuntimeMarker41:
    {
        d3d12event::RuntimeMarker41Args args = {};
        result = GetEventData(pEventRecord, L"pID3D12CommandQueue", &args.pID3D12CommandQueue);
        result = GetEventData(pEventRecord, L"ContextCount", &args.ContextCount);
        args.Contexts.resize(args.ContextCount);
        result = GetEventDataEx(pEventRecord, L"Contexts", args.Contexts.data(), args.ContextCount * sizeof(args.Contexts[0]));
        result = GetEventData(pEventRecord, L"LoopIteration", &args.LoopIteration);
        result = GetEventData(pEventRecord, L"SubmitCommandCBSequence", &args.SubmitCommandCBSequence);
        result = GetEventData(pEventRecord, L"FirstAPISequenceNumberHigh", &args.FirstAPISequenceNumberHigh);
        result = GetEventData(pEventRecord, L"CompletedAPISequenceNumberSize", &args.CompletedAPISequenceNumberSize);
        args.CompletedAPISequenceNumber.resize(args.CompletedAPISequenceNumberSize);
        result = GetEventDataEx(pEventRecord, L"CompletedAPISequenceNumber", args.CompletedAPISequenceNumber.data(), args.CompletedAPISequenceNumberSize * sizeof(args.CompletedAPISequenceNumber[0]));
        break;
    }
    case FenceStart:
    case FenceStop:
    {
        d3d12event::FenceStartArgs args = {};
        result = GetEventData(pEventRecord, L"pID3D12Device", &args.pID3D12Device);
        result = GetEventData(pEventRecord, L"pID3D12Fence", &args.pID3D12Fence);
        result = GetEventData(pEventRecord, L"pDXGKFence", &args.pDXGKFence);
        args.isStart = (id == FenceStart);
        break;
    }
    case Marker38:
    {
        d3d12event::Marker38Args args = {};
        uint32_t DataSize;
        result = GetEventData(pEventRecord, L"pID3D12CommandList", &args.pID3D12CommandList);
        result = GetEventData(pEventRecord, L"APISequenceNumber", &args.APISequenceNumber);
        result = GetEventData(pEventRecord, L"Metadata", &args.Metadata);
        result = GetEventData(pEventRecord, L"DataSize", &DataSize);
        //args.Data.resize(DataSize);
        result = GetEventData(pEventRecord, L"Data", &args.Data);

        break;
    }
    }
    (pmConsumer);
}