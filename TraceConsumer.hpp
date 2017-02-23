#pragma once
#include "CommonIncludes.hpp"

struct ITraceConsumer {
    virtual void OnEventRecord(_In_ PEVENT_RECORD pEventRecord) = 0;
    virtual bool ContinueProcessing() = 0;
    
    // Time of the first event of the trace
    uint64_t mTraceStartTime = 0;
};

class TraceEventInfo
{
public:
    TraceEventInfo(PEVENT_RECORD pEvent)
        : pEvent(pEvent) {
        unsigned long bufferSize = 0;
        auto result = TdhGetEventInformation(pEvent, 0, nullptr, nullptr, &bufferSize);
        if (result == ERROR_INSUFFICIENT_BUFFER) {
            pInfo = reinterpret_cast<TRACE_EVENT_INFO*>(operator new(bufferSize));
            result = TdhGetEventInformation(pEvent, 0, nullptr, pInfo, &bufferSize);
        }
        if (result != ERROR_SUCCESS) {
            throw std::exception("Unexpected error from TdhGetEventInformation.", result);
        }
    }
    TraceEventInfo(const TraceEventInfo&) = delete;
    TraceEventInfo& operator=(const TraceEventInfo&) = delete;
    TraceEventInfo(TraceEventInfo&& o) {
        *this = std::move(o);
    }
    TraceEventInfo& operator=(TraceEventInfo&& o) {
        if (pInfo) {
            operator delete(pInfo);
        }
        pInfo = o.pInfo;
        pEvent = o.pEvent;
        o.pInfo = nullptr;
        return *this;
    }

    ~TraceEventInfo() {
        operator delete(pInfo);
        pInfo = nullptr;
    }

    void GetData(PCWSTR name, byte* outData, uint32_t dataSize) {
        PROPERTY_DATA_DESCRIPTOR descriptor;
        descriptor.ArrayIndex = 0;
        descriptor.PropertyName = reinterpret_cast<unsigned long long>(name);
        auto result = TdhGetProperty(pEvent, 0, nullptr, 1, &descriptor, dataSize, outData);
        if (result != ERROR_SUCCESS) {
            throw std::exception("Unexpected error from TdhGetProperty.", result);
        }
    }

    uint32_t GetDataSize(PCWSTR name) {
        PROPERTY_DATA_DESCRIPTOR descriptor;
        descriptor.ArrayIndex = 0;
        descriptor.PropertyName = reinterpret_cast<unsigned long long>(name);
        ULONG size = 0;
        auto result = TdhGetPropertySize(pEvent, 0, nullptr, 1, &descriptor, &size);
        if (result != ERROR_SUCCESS) {
            throw std::exception("Unexpected error from TdhGetPropertySize.", result);
        }
        return size;
    }

    template <typename T>
    T GetData(PCWSTR name) {
        T local;
        GetData(name, reinterpret_cast<byte*>(&local), sizeof(local));
        return local;
    }

    uint64_t GetPtr(PCWSTR name) {
        if (pEvent->EventHeader.Flags & EVENT_HEADER_FLAG_32_BIT_HEADER) {
            return GetData<uint32_t>(name);
        }
        else if (pEvent->EventHeader.Flags & EVENT_HEADER_FLAG_64_BIT_HEADER) {
            return GetData<uint64_t>(name);
        }
        return 0;
    }

private:
    TRACE_EVENT_INFO* pInfo;
    EVENT_RECORD* pEvent;
};

class MultiTraceConsumer : public ITraceConsumer
{

public:
    void AddTraceConsumer(ITraceConsumer* pConsumer)
    {
        if (pConsumer != nullptr)
        {
            mTraceConsumers.push_back(pConsumer);
        }
    }

    virtual void OnEventRecord(_In_ PEVENT_RECORD pEventRecord);
    virtual bool ContinueProcessing();

private:
    std::vector<ITraceConsumer*> mTraceConsumers;
};
