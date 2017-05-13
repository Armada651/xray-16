#ifndef xrD3DDefs_included
#define xrD3DDefs_included
#pragma once

#if defined(USE_DX11) || defined(USE_DX10)
#include "Layers/xrRenderDX10/DXCommonTypes.h"
#else // USE_DX10

typedef IDirect3DVertexShader9 ID3DVertexShader;
typedef IDirect3DPixelShader9 ID3DPixelShader;
typedef ID3DXBuffer ID3DBlob;
typedef D3DXMACRO D3D_SHADER_MACRO;
typedef IDirect3DQuery9 ID3DQuery;
typedef D3DVIEWPORT9 D3D_VIEWPORT;
typedef ID3DXInclude ID3DInclude;
typedef IDirect3DTexture9 ID3DTexture2D;
typedef IDirect3DSurface9 ID3DRenderTargetView;
typedef IDirect3DSurface9 ID3DDepthStencilView;
typedef IDirect3DBaseTexture9 ID3DBaseTexture;
typedef D3DSURFACE_DESC D3D_TEXTURE2D_DESC;
typedef IDirect3DVertexBuffer9 ID3DVertexBuffer;
typedef IDirect3DIndexBuffer9 ID3DIndexBuffer;
typedef IDirect3DVolumeTexture9 ID3DTexture3D;
typedef IDirect3DStateBlock9 ID3DState;

#define DX10_ONLY(expr) do {} while (0)

#endif // USE_DX10

#ifdef USE_VK
typedef VkBuffer D3DBuffer;
typedef VkBuffer D3DVertexBuffer;
typedef VkBuffer D3DIndexBuffer;
typedef VkShaderModule D3DVertexShader;
typedef VkShaderModule D3DGeometryShader;
typedef VkShaderModule D3DPixelShader;
typedef VkShaderModule D3DHullShader;
typedef VkShaderModule D3DDomainShader;
typedef VkShaderModule D3DComputeShader;
#else
#if defined(USE_DX11) || defined(USE_DX10)
typedef ID3DBuffer* D3DBuffer;
#endif
typedef ID3DVertexBuffer* D3DVertexBuffer;
typedef ID3DIndexBuffer* D3DIndexBuffer;
typedef ID3DVertexShader* D3DVertexShader;
#if defined(USE_DX11) || defined(USE_DX10)
typedef ID3DGeometryShader* D3DGeometryShader;
#endif
#ifdef USE_DX11
typedef ID3D11HullShader* D3DHullShader;
typedef ID3D11DomainShader* D3DDomainShader;
typedef ID3D11ComputeShader* D3DComputeShader;
#endif
typedef ID3DPixelShader* D3DPixelShader;
#endif

#endif // xrD3DDefs_included
