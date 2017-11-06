#pragma once

#include <vector>
#include <string>

namespace d3d12event
{
struct Marker38Args
{
    uint64_t pID3D12CommandList;
    uint64_t APISequenceNumber;
    uint32_t Metadata;
    std::string Data;
};

struct CommandListStartArgs
{
    uint64_t pID3D12Device;
    uint64_t pID3D12CommandList;
    uint64_t SequenceNumber;
    uint32_t type;
};

struct ExecuteCommandListStartArgs
{
    uint64_t pID3D12CommandQueue;
    uint64_t pID3D12CommandList;
};

struct RuntimeMarkerArgs
{
    uint64_t CPUFrequency;
    uint64_t FirstAPISequenceNumber;
    uint64_t pID3D12CommandList;
    uint32_t CPUTimeHigh;
    uint8_t ThreadIDCount;
    std::vector<uint32_t> ThreadIDs;
    uint32_t DataSize;
    std::vector<uint8_t> Data;
};

struct FenceStartArgs
{
    uint64_t pID3D12Device;
    uint64_t pID3D12Fence;
    uint64_t pDXGKFence;
    bool isStart;
};

struct NameArgs
{
    uint64_t pObject;
    std::string OldDebugObjectName;
    std::string NewDebugObjectName;
};

}
