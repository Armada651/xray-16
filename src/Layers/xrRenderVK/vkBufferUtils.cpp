#include "stdafx.h"
#include "vkBufferUtils.h"

namespace vkBufferUtils
{
VkResult IC CreateBuffer(ID3DBuffer** ppBuffer, const void* pData, UINT DataSize, bool bImmutable, bool bIndexBuffer)
{
    ID3DBuffer* buffer = new ID3DBuffer();
    VkResult res = buffer->Create(pData, DataSize, bIndexBuffer ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    if (res != VK_SUCCESS)
    {
        delete buffer;
        return res;
    }

    buffer->AddRef();
    *ppBuffer = buffer;
    return res;
}

VkResult CreateVertexBuffer(ID3DVertexBuffer** ppBuffer, const void* pData, UINT DataSize, bool bImmutable)
{
    return CreateBuffer(ppBuffer, pData, DataSize, bImmutable, false);
}

VkResult CreateIndexBuffer(ID3DIndexBuffer** ppBuffer, const void* pData, UINT DataSize, bool bImmutable)
{
    return CreateBuffer(ppBuffer, pData, DataSize, bImmutable, true);
}

VkResult CreateConstantBuffer(ID3DBuffer** ppBuffer, UINT DataSize)
{
    ID3DBuffer* buffer = new ID3DBuffer();
    VkResult res = buffer->Create(nullptr, DataSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    if (res != VK_SUCCESS)
    {
        delete buffer;
        return res;
    }

    buffer->AddRef();
    *ppBuffer = buffer;
    return res;
}

struct VertexFormatPairs
{
    D3DDECLTYPE m_dx9FMT;
    DXGI_FORMAT m_dx10FMT;
};

VertexFormatPairs VertexFormatList[] = {{D3DDECLTYPE_FLOAT1, DXGI_FORMAT_R32_FLOAT},
    {D3DDECLTYPE_FLOAT2, DXGI_FORMAT_R32G32_FLOAT}, {D3DDECLTYPE_FLOAT3, DXGI_FORMAT_R32G32B32_FLOAT},
    {D3DDECLTYPE_FLOAT4, DXGI_FORMAT_R32G32B32A32_FLOAT},
    {D3DDECLTYPE_D3DCOLOR,
        DXGI_FORMAT_R8G8B8A8_UNORM}, // Warning. Explicit RGB component swizzling is nesessary	//	Not available
    {D3DDECLTYPE_UBYTE4, DXGI_FORMAT_R8G8B8A8_UINT}, // Note: Shader gets UINT values, but if Direct3D 9 style integral
    // floats are needed (0.0f, 1.0f... 255.f), UINT can just be converted
    // to float32 in shader.
    {D3DDECLTYPE_SHORT2,
        DXGI_FORMAT_R16G16_SINT}, // Note: Shader gets SINT values, but if Direct3D 9 style integral floats
    // are needed, SINT can just be converted to float32 in shader.
    {D3DDECLTYPE_SHORT4,
        DXGI_FORMAT_R16G16B16A16_SINT}, // Note: Shader gets SINT values, but if Direct3D 9 style integral
    // floats are needed, SINT can just be converted to float32 in
    // shader.
    {D3DDECLTYPE_UBYTE4N, DXGI_FORMAT_R8G8B8A8_UNORM},
    {D3DDECLTYPE_SHORT2N, DXGI_FORMAT_R16G16_SNORM}, {D3DDECLTYPE_SHORT4N, DXGI_FORMAT_R16G16B16A16_SNORM},
    {D3DDECLTYPE_USHORT2N, DXGI_FORMAT_R16G16_UNORM}, {D3DDECLTYPE_USHORT4N, DXGI_FORMAT_R16G16B16A16_UNORM},
    // D3DDECLTYPE_UDEC3 Not available
    // D3DDECLTYPE_DEC3N Not available
    {D3DDECLTYPE_FLOAT16_2, DXGI_FORMAT_R16G16_FLOAT}, {D3DDECLTYPE_FLOAT16_4, DXGI_FORMAT_R16G16B16A16_FLOAT}};

DXGI_FORMAT ConvertVertexFormat(D3DDECLTYPE dx9FMT)
{
    int arrayLength = sizeof(VertexFormatList) / sizeof(VertexFormatList[0]);
    for (int i = 0; i < arrayLength; ++i)
    {
        if (VertexFormatList[i].m_dx9FMT == dx9FMT)
            return VertexFormatList[i].m_dx10FMT;
    }

    VERIFY(!"ConvertVertexFormat didn't find appropriate dx10 vertex format!");
    return DXGI_FORMAT_UNKNOWN;
}

struct VertexSemanticPairs
{
    D3DDECLUSAGE m_dx9Semantic;
    LPCSTR m_dx10Semantic;
};

VertexSemanticPairs VertexSemanticList[] = {
    {D3DDECLUSAGE_POSITION, "POSITION"}, //	0
    {D3DDECLUSAGE_BLENDWEIGHT, "BLENDWEIGHT"}, // 1
    {D3DDECLUSAGE_BLENDINDICES, "BLENDINDICES"}, // 2
    {D3DDECLUSAGE_NORMAL, "NORMAL"}, // 3
    {D3DDECLUSAGE_PSIZE, "PSIZE"}, // 4
    {D3DDECLUSAGE_TEXCOORD, "TEXCOORD"}, // 5
    {D3DDECLUSAGE_TANGENT, "TANGENT"}, // 6
    {D3DDECLUSAGE_BINORMAL, "BINORMAL"}, // 7
    // D3DDECLUSAGE_TESSFACTOR,    // 8
    {D3DDECLUSAGE_POSITIONT, "POSITIONT"}, // 9
    {D3DDECLUSAGE_COLOR, "COLOR"}, // 10
    // D3DDECLUSAGE_FOG,           // 11
    // D3DDECLUSAGE_DEPTH,         // 12
    // D3DDECLUSAGE_SAMPLE,        // 13
};

LPCSTR ConvertSemantic(D3DDECLUSAGE Semantic)
{
    int arrayLength = sizeof(VertexSemanticList) / sizeof(VertexSemanticList[0]);
    for (int i = 0; i < arrayLength; ++i)
    {
        if (VertexSemanticList[i].m_dx9Semantic == Semantic)
            return VertexSemanticList[i].m_dx10Semantic;
    }

    VERIFY(!"ConvertSemantic didn't find appropriate dx10 input semantic!");
    return 0;
}

void ConvertVertexDeclaration(const xr_vector<D3DVERTEXELEMENT9>& declIn, xr_vector<D3D_INPUT_ELEMENT_DESC>& declOut)
{
    int iDeclSize = declIn.size() - 1;
    declOut.resize(iDeclSize + 1);

    for (int i = 0; i < iDeclSize; ++i)
    {
        const D3DVERTEXELEMENT9& descIn = declIn[i];
        D3D_INPUT_ELEMENT_DESC& descOut = declOut[i];

        descOut.SemanticName = ConvertSemantic((D3DDECLUSAGE)descIn.Usage);
        descOut.SemanticIndex = descIn.UsageIndex;
        descOut.Format = ConvertVertexFormat((D3DDECLTYPE)descIn.Type);
        descOut.InputSlot = descIn.Stream;
        descOut.AlignedByteOffset = descIn.Offset;
        descOut.InputSlotClass = D3D_INPUT_PER_VERTEX_DATA;
        descOut.InstanceDataStepRate = 0;
    }

    ZeroMemory(&declOut[iDeclSize], sizeof(declOut[iDeclSize]));
}
};
