#ifndef vkBufferUtils_included
#define vkBufferUtils_included
#pragma once
#if defined(USE_VK)

namespace vkBufferUtils
{
VkResult CreateVertexBuffer(ID3DVertexBuffer** ppBuffer, const void* pData, UINT DataSize, bool bImmutable = true);
VkResult CreateIndexBuffer(ID3DIndexBuffer** ppBuffer, const void* pData, UINT DataSize, bool bImmutable = true);
VkResult CreateConstantBuffer(ID3DBuffer** ppBuffer, UINT DataSize);
void ConvertVertexDeclaration(const xr_vector<D3DVERTEXELEMENT9>& declIn, xr_vector<D3D_INPUT_ELEMENT_DESC>& declOut);
};

#endif //	USE_VK
#endif //	vkBufferUtils_included
