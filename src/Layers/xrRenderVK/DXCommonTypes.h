#pragma once

#include <Unknwn.h>
#include <atomic>

class CUnknown : public IUnknown
{
public:
    HRESULT QueryInterface(REFIID riid, void** ppvObject) { return ppvObject ? E_NOINTERFACE : E_POINTER; }
    ULONG AddRef() { return ++m_ulRef; }
    ULONG Release()
    {
        ULONG ref = --m_ulRef;
        if (ref == 0)
            delete this;
        return ref;
    }

    CUnknown() : m_ulRef() {}
    virtual ~CUnknown() {}

private:
    std::atomic_ulong m_ulRef;
};

class DXBuffer : public CUnknown
{
public:
    DXBuffer();
    virtual ~DXBuffer();

    VkResult Create(const void* pData, VkDeviceSize DataSize, VkBufferUsageFlags Usage);

    VkBuffer m_buffer;
    VkDeviceMemory m_memory;

private:
    bool MemoryTypeFromProperties(VkPhysicalDeviceMemoryProperties properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
};

typedef DXBuffer ID3DBuffer;
typedef DXBuffer ID3DVertexBuffer;
typedef DXBuffer ID3DIndexBuffer;
