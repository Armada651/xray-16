#pragma once

#include <Unknwn.h>
#include <atomic>

class CUnknown : public IUnknown
{
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) { return ppvObject ? E_NOINTERFACE : E_POINTER; }
    ULONG STDMETHODCALLTYPE AddRef() { return ++m_ulRef; }
    ULONG STDMETHODCALLTYPE Release()
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

class DXShader : public CUnknown
{
public:
    DXShader();
    virtual ~DXShader();

    VkResult Create(const uint32_t* pCode, size_t codeSize);

    VkShaderModule m_shader;
};

typedef DXShader ID3DVertexShader;
typedef DXShader ID3DGeometryShader;
typedef DXShader ID3DPixelShader;
typedef DXShader ID3DHullShader;
typedef DXShader ID3DDomainShader;
typedef DXShader ID3DComputeShader;
