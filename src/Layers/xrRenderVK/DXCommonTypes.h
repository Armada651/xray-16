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
private:
    DXBuffer();

public:
    virtual ~DXBuffer();

    static VkResult Create(const void* pData, VkDeviceSize dataSize,
        VkBufferUsageFlags usage, DXBuffer** outBuffer);

    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
};

typedef DXBuffer ID3DBuffer;
typedef DXBuffer ID3DVertexBuffer;
typedef DXBuffer ID3DIndexBuffer;

class DXShader : public CUnknown
{
private:
    DXShader();

public:
    virtual ~DXShader();

    static VkResult Create(const uint32_t* pCode, size_t codeSize, DXShader** outShader);

    VkShaderModule m_shader;
};

typedef DXShader ID3DVertexShader;
typedef DXShader ID3DGeometryShader;
typedef DXShader ID3DPixelShader;
typedef DXShader ID3DHullShader;
typedef DXShader ID3DDomainShader;
typedef DXShader ID3DComputeShader;

class DXTexture : public CUnknown
{
private:
    DXTexture();

public:
    virtual ~DXTexture();

    static VkResult Create(VkImageCreateInfo* createInfo, const void* pData,
        VkDeviceSize dataSize, bool bStaging, DXTexture** outTexture);

    VkImage m_image;
    VkDeviceMemory m_memory;
    VkBuffer m_stagingBuffer;
    VkDeviceMemory m_stagingMemory;
    VkCommandBuffer m_copyCmd;

    VkImageCreateInfo m_createInfo;
};

typedef DXTexture ID3DBaseTexture;
typedef VkImageCreateInfo D3D_TEXTURE2D_DESC;
