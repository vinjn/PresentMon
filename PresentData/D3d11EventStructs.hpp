#pragma once

#include <vector>
#include <string>

namespace d3d11event
{
struct MarkerArgs
{
    uint64_t APISequenceNumber;
    std::string Label; // UnicodeString
    int32_t Data;
};

struct CommandListStartArgs
{
    uint64_t pID3D11DeviceContext;
    uint64_t pID3D11CommandList;
};

struct RuntimeMarkerArgs
{
    uint64_t CPUFrequency;
    uint64_t FirstAPISequenceNumber;
    uint64_t pID3D11DeviceContext;
    uint32_t CPUTimeHigh;
    uint32_t ThreadIDCount;
    std::vector<uint8_t> ThreadIDs;
    uint32_t DataSize;
    std::vector<uint8_t> Data;
};

struct RuntimeMarker41Args
{
    uint64_t pIDXGIDevice;
    uint32_t hContext;
    uint8_t BroadcastContexts;
    std::vector<uint32_t> hBroadcastContexts; // BroadcastContexts
    uint32_t MarkerLogType; // None, Profile
    uint32_t RenderCBSequenceNumber;
    uint32_t FirstAPISequenceNumberHigh;

    uint32_t CompletedAPISequenceNumberLow0Size;
    uint32_t CompletedAPISequenceNumberLow1Size;
    uint32_t BegunAPISequenceNumberLow0Size;
    uint32_t BegunAPISequenceNumberLow1Size;
    std::vector<uint32_t> CompletedAPISequenceNumberLow0; // CompletedAPISequenceNumberLow0Size
    std::vector<uint32_t> CompletedAPISequenceNumberLow1; // CompletedAPISequenceNumberLow1Size
    std::vector<uint32_t> BegunAPISequenceNumberLow0; // BegunAPISequenceNumberLow0Size
    std::vector<uint32_t> BegunAPISequenceNumberLow1; // BegunAPISequenceNumberLow1Size
};

struct CustomDriverMarkerArgs
{
    uint64_t InsertionAPISequenceNumber;
    uint64_t pID3D11DeviceContext;
    uint32_t hContext;
    int32_t Index;
    int32_t StringIndex;
    std::string MarkerDescription; // UnicodeString
};

struct CustomDriverMarker46Args
{
    uint64_t pID3D11Device;
    int32_t StringIndex;
    std::string Description; // UnicodeString
};

struct NameArgs
{
    uint64_t pObject;
    std::string OldDebugObjectName;
    std::string NewDebugObjectName;
};

}
