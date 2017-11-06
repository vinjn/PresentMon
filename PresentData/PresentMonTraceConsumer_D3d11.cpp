#define NOMINMAX
#include <algorithm>
#include <d3d11.h>
#include <dxgi.h>

#include "PresentMonTraceConsumer.hpp"
#include "TraceConsumer.hpp"
#include "D3d11EventStructs.hpp"

using namespace std;

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
