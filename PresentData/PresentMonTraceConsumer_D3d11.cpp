#define NOMINMAX
#include <algorithm>
#include <d3d11.h>
#include <dxgi.h>

#include "PresentMonTraceConsumer.hpp"
#include "TraceConsumer.hpp"
#include "D3d11EventStructs.hpp"

using namespace std;

enum D3d11Evt
{
    Name = 1,
    NameDCStart = 2,
    DeviceStart = 3,
    DeviceStop = 4,
    DeviceDCStart = 5,
    BufferStart = 6,
    BufferStop = 7,
    BufferDCStart = 8,
    Texture1DStart = 9,
    Texture1DStop = 10,
    Texture1DDCStart = 11,
    Texture2DStart = 12,
    Texture2DStop = 13,
    Texture2DDCStart = 14,
    Texture3DStart = 15,
    Texture3DStop = 16,
    Texture3DDCStart = 17,
    Texture2D = 18,
    Device = 19,
    D3D11CoreCreateDeviceStart = 20,
    D3D11CoreCreateDeviceStop = 21,
    KMDDeviceCreateStart = 22,
    KMDDeviceCreateStop = 23,
    UMDDeviceCreateStart = 24,
    UMDDeviceCreateStop = 25,
    D3DSupplemental = 26,
    QueryInterface = 27,
    AddRef = 28,
    Release = 29,
    Destruction = 30,
    UMD10CreateDeviceStart = 31,
    UMD10CreateDeviceStop = 32,
    UMDOpenAdapterStart = 33,
    UMDOpenAdapterStop = 34,
    ContextStart = 35,
    ContextStop = 36,
    ContextDCStart = 37,
    Marker = 38,
    RuntimeMarker = 39,
    RuntimeDroppedCall = 40,
    RuntimeMarker41 = 41,
    CommandListStart = 42,
    CommandListStop = 43,
    CommandListDCStart = 44,
    CustomDriverMarker = 45,
    CustomDriverMarker46 = 46,
    IDXGIDevice2_OfferResourcesStart = 1714,
    IDXGIDevice2_OfferResourcesStop = 1715,
    IDXGIDevice2_ReclaimResourcesStart = 1716,
    IDXGIDevice2_ReclaimResourcesStop = 1717,
    ID3D11VideoDevice_CreateVideoProcessorStart = 1720,
    ID3D11VideoDevice_CreateVideoProcessorStop = 1721,
    ID3D11VideoDevice_CreateVideoDecoderStart = 1722,
    ID3D11VideoDevice_CreateVideoDecoderStop = 1723,
    ID3D11VideoProcessor_ReleaseStart = 1728,
    ID3D11VideoProcessor_ReleaseStop = 1729,
    ID3D11VideoDecoder_ReleaseStart = 1734,
    ID3D11VideoDecoder_ReleaseStop = 1735,
    ID3D11VideoContext_VideoProcessorBltStart = 1778,
    ID3D11VideoContext_VideoProcessorBltStop = 1779,
    ID3D11VideoContext_GetDecoderBufferStart = 1780,
    ID3D11VideoContext_GetDecoderBufferStop = 1781,
    ID3D11VideoContext_DecoderBeginFrameStart = 1784,
    ID3D11VideoContext_DecoderBeginFrameStop = 1785,
    ID3D11VideoContext_DecoderEndFrameStart = 1786,
    ID3D11VideoContext_DecoderEndFrameStop = 1787,
    ID3D11VideoContext_SubmitDecoderBuffersStart = 1788,
    ID3D11VideoContext_SubmitDecoderBuffersStop = 1789,
    IDXGIDevice3_TrimStart = 1804,
    IDXGIDevice3_TrimStop = 1805,
    ID3D11VideoDevice_CreateCryptoSessionStart = 1806,
    ID3D11VideoDevice_CreateCryptoSessionStop = 1807,
    ID3D11VideoContext_NegotiateCryptoSessionKeyExchangeStart = 1808,
    ID3D11VideoContext_NegotiateCryptoSessionKeyExchangeStop = 1809,
    ID3D11VideoContext_VideoProcessorGetBehaviorHintsStart = 1810,
    ID3D11VideoContext_VideoProcessorGetBehaviorHintsStop = 1811,
    CryptoSessionTeardownCount = 1812,
    RecoverTeardown = 1813,
    ID3D11VideoContext_VideoProcessorBltParameters = 1814,
    ID3D11VideoDevice_DestroyCryptoSessionStart = 1815,
    ID3D11VideoDevice_DestroyCryptoSessionStop = 1816,
    ID3D11VideoContext_GetDataForNewHardwareKey = 1817,
};

void HandleD3D11Event(EVENT_RECORD* pEventRecord, PMTraceConsumer* pmConsumer)
{
    auto const& hdr = pEventRecord->EventHeader;
    
    if (0)
    {
        fprintf(stderr, "Id: %d, Opcode: %d\n", hdr.EventDescriptor.Id, hdr.EventDescriptor.Opcode);
        bool result = true;
        result = GetEventData<bool>(pEventRecord, L"WTF");
        (result);
    }

    switch (hdr.EventDescriptor.Id)
    {
    case 0:break;
    }
    (pmConsumer);
}
