#include "stdafx.h"
#include "vkStateUtils.h"
#include "Layers/xrRender/Utils/dxHashHelper.h"

namespace vkStateUtils
{

D3D_CULL_MODE ConvertCullMode(D3DCULL Mode)
{
	switch (Mode)
	{
	case D3DCULL_NONE:
		return D3D_CULL_NONE;
	case D3DCULL_CW:
		return D3D_CULL_FRONT;
	case D3DCULL_CCW:
		return D3D_CULL_BACK;
	default:
		VERIFY(!"Unexpected cull mode!");
		return D3D_CULL_NONE;
	}
}

D3D_COMPARISON_FUNC ConvertCmpFunction(D3DCMPFUNC Func)
{
	switch (Func)
	{
	case D3DCMP_NEVER:
		return D3D_COMPARISON_NEVER;
	case D3DCMP_LESS:
		return D3D_COMPARISON_LESS;
	case D3DCMP_EQUAL:
		return D3D_COMPARISON_EQUAL;
	case D3DCMP_LESSEQUAL:
		return D3D_COMPARISON_LESS_EQUAL;
	case D3DCMP_GREATER:
		return D3D_COMPARISON_GREATER;
	case D3DCMP_NOTEQUAL:
		return D3D_COMPARISON_NOT_EQUAL;
	case D3DCMP_GREATEREQUAL:
		return D3D_COMPARISON_GREATER_EQUAL;
	case D3DCMP_ALWAYS:
		return D3D_COMPARISON_ALWAYS;
	default:
		VERIFY(!"ConvertCmpFunction can't convert argument!");
		return D3D_COMPARISON_ALWAYS;
	}
}

D3D_STENCIL_OP ConvertStencilOp(D3DSTENCILOP Op)
{
	switch (Op)
	{
	case D3DSTENCILOP_KEEP:
		return D3D_STENCIL_OP_KEEP;
	case D3DSTENCILOP_ZERO:
		return D3D_STENCIL_OP_ZERO;
	case D3DSTENCILOP_REPLACE:
		return D3D_STENCIL_OP_REPLACE;
	case D3DSTENCILOP_INCRSAT:
		return D3D_STENCIL_OP_INCR_SAT;
	case D3DSTENCILOP_DECRSAT:
		return D3D_STENCIL_OP_DECR_SAT;
	case D3DSTENCILOP_INVERT:
		return D3D_STENCIL_OP_INVERT;
	case D3DSTENCILOP_INCR:
		return D3D_STENCIL_OP_INCR;
	case D3DSTENCILOP_DECR:
		return D3D_STENCIL_OP_DECR;
	default:
		VERIFY(!"ConvertStencilOp can't convert argument!");
		return D3D_STENCIL_OP_KEEP;
	}
}

D3D_BLEND ConvertBlendArg(D3DBLEND Arg)
{
	switch (Arg)
	{
	case D3DBLEND_ZERO:
		return D3D_BLEND_ZERO;
	case D3DBLEND_ONE:
		return D3D_BLEND_ONE;
	case D3DBLEND_SRCCOLOR:
		return D3D_BLEND_SRC_COLOR;
	case D3DBLEND_INVSRCCOLOR:
		return D3D_BLEND_INV_SRC_COLOR;
	case D3DBLEND_SRCALPHA:
		return D3D_BLEND_SRC_ALPHA;
	case D3DBLEND_INVSRCALPHA:
		return D3D_BLEND_INV_SRC_ALPHA;
	case D3DBLEND_DESTALPHA:
		return D3D_BLEND_DEST_ALPHA;
	case D3DBLEND_INVDESTALPHA:
		return D3D_BLEND_INV_DEST_ALPHA;
	case D3DBLEND_DESTCOLOR:
		return D3D_BLEND_DEST_COLOR;
	case D3DBLEND_INVDESTCOLOR:
		return D3D_BLEND_INV_DEST_COLOR;
	case D3DBLEND_SRCALPHASAT:
		return D3D_BLEND_SRC_ALPHA_SAT;
	//case D3DBLEND_BOTHSRCALPHA:
	//	return ;
	//case D3DBLEND_BOTHINVSRCALPHA:
	//	return ;
	case D3DBLEND_BLENDFACTOR:
		return D3D_BLEND_BLEND_FACTOR;
	case D3DBLEND_INVBLENDFACTOR:
		return D3D_BLEND_INV_BLEND_FACTOR;
	case D3DBLEND_SRCCOLOR2:
		return D3D_BLEND_SRC1_COLOR;
	case D3DBLEND_INVSRCCOLOR2:
		return D3D_BLEND_INV_SRC1_COLOR;
		default:
			VERIFY(!"ConvertBlendArg can't convert argument!");
			return D3D_BLEND_ONE;
	}
}

D3D_BLEND_OP ConvertBlendOp(D3DBLENDOP Op)
{
	switch (Op)
	{
	case D3DBLENDOP_ADD:
		return D3D_BLEND_OP_ADD;
	case D3DBLENDOP_SUBTRACT:
		return D3D_BLEND_OP_SUBTRACT;
	case D3DBLENDOP_REVSUBTRACT:
		return D3D_BLEND_OP_REV_SUBTRACT;
	case D3DBLENDOP_MIN:
		return D3D_BLEND_OP_MIN;
	case D3DBLENDOP_MAX:
		return D3D_BLEND_OP_MAX;
	default:
		VERIFY(!"ConvertBlendOp can't convert argument!");
		return D3D_BLEND_OP_ADD;
	}
}

D3D_TEXTURE_ADDRESS_MODE	ConvertTextureAddressMode(D3DTEXTUREADDRESS Mode)
{
	switch(Mode)
	{
	case D3DTADDRESS_WRAP:
		return D3D_TEXTURE_ADDRESS_WRAP;
	case D3DTADDRESS_MIRROR:
		return D3D_TEXTURE_ADDRESS_MIRROR;
	case D3DTADDRESS_CLAMP:
		return D3D_TEXTURE_ADDRESS_CLAMP;
	case D3DTADDRESS_BORDER:
		return D3D_TEXTURE_ADDRESS_BORDER;
	case D3DTADDRESS_MIRRORONCE:
		return D3D_TEXTURE_ADDRESS_MIRROR_ONCE;
	default:
		VERIFY(!"ConvertTextureAddressMode can't convert argument!");
		return D3D_TEXTURE_ADDRESS_CLAMP;
	}
}

void ResetDescription( D3D_RASTERIZER_DESC &desc )
{
	ZeroMemory(&desc, sizeof(desc));
	desc.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

	desc.polygonMode = VK_POLYGON_MODE_FILL;
	desc.cullMode = VK_CULL_MODE_BACK_BIT;
	desc.frontFace = VK_FRONT_FACE_CLOCKWISE;
	desc.depthBiasEnable = VK_FALSE;
	desc.depthBiasConstantFactor = 0;
	desc.depthBiasClamp = 0;
	desc.depthBiasSlopeFactor = 0;
	desc.lineWidth = 1.0f;
	desc.depthClampEnable = VK_FALSE;
	desc.rasterizerDiscardEnable = VK_FALSE;
}

void ResetDescription( D3D_DEPTH_STENCIL_DESC &desc )
{
	ZeroMemory(&desc, sizeof(desc));
	desc.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	desc.depthTestEnable = VK_TRUE;
	desc.depthWriteEnable = VK_TRUE;
	desc.depthCompareOp = VK_COMPARE_OP_LESS;
	desc.depthBoundsTestEnable = VK_FALSE;
	desc.stencilTestEnable = VK_TRUE;

	if (!RImplementation.o.dx10_msaa)
	{
		desc.front.compareMask = desc.back.compareMask = 0xFF;
		desc.front.writeMask = desc.back.writeMask = 0xFF;
	}
	else
	{
		desc.front.compareMask = desc.back.compareMask = 0x7F;
		desc.front.writeMask = desc.back.writeMask = 0x7F;
	}

	desc.front.failOp = D3D_STENCIL_OP_KEEP;
	desc.front.depthFailOp = D3D_STENCIL_OP_KEEP;
	desc.front.passOp = D3D_STENCIL_OP_KEEP;
	desc.front.compareOp = D3D_COMPARISON_ALWAYS;

	desc.back.failOp = D3D_STENCIL_OP_KEEP;
	desc.back.depthFailOp = D3D_STENCIL_OP_KEEP;
	desc.back.passOp = D3D_STENCIL_OP_KEEP;
	desc.back.compareOp = D3D_COMPARISON_ALWAYS;
}

void ResetDescription( D3D_BLEND_DESC &desc )
{
	ZeroMemory(&desc, sizeof(desc));
	desc.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

	desc.logicOpEnable = VK_FALSE;
	desc.logicOp = VK_LOGIC_OP_NO_OP;

	VkPipelineColorBlendAttachmentState att;
	att.srcColorBlendFactor = D3D_BLEND_ONE;
	att.dstColorBlendFactor = D3D_BLEND_ZERO;
	att.colorBlendOp = D3D_BLEND_OP_ADD;
	att.srcAlphaBlendFactor = D3D_BLEND_ONE;
	att.dstAlphaBlendFactor = D3D_BLEND_ZERO;
	att.alphaBlendOp = D3D_BLEND_OP_ADD;
	att.blendEnable = VK_FALSE;
	att.colorWriteMask = D3D_COLOR_WRITE_ENABLE_ALL;

	for (int i = 0; i<desc.attachmentCount; ++i)
		memcpy((void*)(desc.pAttachments + i), &att, sizeof(att));
}

void ResetDescription( D3D_SAMPLER_DESC &desc )
{
	ZeroMemory(&desc, sizeof(desc));
	desc.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	desc.minFilter = VK_FILTER_LINEAR;
	desc.magFilter = VK_FILTER_LINEAR;
	desc.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	desc.addressModeU = D3D_TEXTURE_ADDRESS_CLAMP;
	desc.addressModeV = D3D_TEXTURE_ADDRESS_CLAMP;
	desc.addressModeW = D3D_TEXTURE_ADDRESS_CLAMP;
	desc.mipLodBias = 0;
	desc.anisotropyEnable = VK_FALSE;
	desc.maxAnisotropy = 1;
	desc.compareEnable = VK_FALSE;
	desc.compareOp = D3D_COMPARISON_NEVER;
	desc.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	desc.minLod = -FLT_MAX;
	desc.maxLod = FLT_MAX;
	desc.unnormalizedCoordinates = VK_FALSE;
}

bool operator==(const D3D_RASTERIZER_DESC &desc1, const D3D_RASTERIZER_DESC &desc2)
{
	if (desc1.polygonMode != desc2.polygonMode) return false;
	if (desc1.cullMode != desc2.cullMode) return false;
	if (desc1.frontFace != desc2.frontFace) return false;
	if (desc1.depthBiasEnable != desc2.depthBiasEnable) return false;
	if (desc1.depthBiasConstantFactor != desc2.depthBiasConstantFactor) return false;
	if (desc1.depthBiasClamp != desc2.depthBiasClamp) return false;
	if (desc1.depthBiasSlopeFactor != desc2.depthBiasSlopeFactor) return false;
	if (desc1.lineWidth != desc2.lineWidth) return false;
	if (desc1.depthClampEnable != desc2.depthClampEnable) return false;
	if (desc1.rasterizerDiscardEnable != desc2.rasterizerDiscardEnable) return false;

	return true;
}

bool operator==(const D3D_DEPTH_STENCIL_DESC &desc1, const D3D_DEPTH_STENCIL_DESC &desc2)
{
	if ( desc1.depthTestEnable != desc2.depthTestEnable) return false;
	if ( desc1.depthWriteEnable != desc2.depthWriteEnable) return false;
	if ( desc1.depthCompareOp != desc2.depthCompareOp) return false;
	if ( desc1.depthBoundsTestEnable != desc2.depthBoundsTestEnable) return false;
	if ( desc1.stencilTestEnable != desc2.stencilTestEnable) return false;
	if ( desc1.front.compareMask != desc2.front.compareMask) return false;
	if ( desc1.back.compareMask != desc2.back.compareMask) return false;
	if ( desc1.front.writeMask != desc2.front.writeMask) return false;
	if ( desc1.back.writeMask != desc2.back.writeMask) return false;

	if ( desc1.front.failOp != desc2.front.failOp) return false;
	if ( desc1.front.depthFailOp != desc2.front.depthFailOp) return false;
	if ( desc1.front.passOp != desc2.front.passOp) return false;
	if ( desc1.front.compareOp != desc2.front.compareOp) return false;

	if ( desc1.back.failOp != desc2.back.failOp) return false;
	if ( desc1.back.depthFailOp != desc2.back.depthFailOp) return false;
	if ( desc1.back.passOp != desc2.back.passOp) return false;
	if ( desc1.back.compareOp != desc2.back.compareOp) return false;
	
	return true;
}

bool operator==(const D3D_BLEND_DESC &desc1, const D3D_BLEND_DESC &desc2)
{
	if ( desc1.logicOpEnable != desc2.logicOpEnable) return false;
	if ( desc1.logicOp != desc2.logicOp) return false;
	if ( desc1.attachmentCount != desc2.attachmentCount) return false;
	
	for ( int i=0; i<desc1.attachmentCount && i<desc2.attachmentCount; ++i)
	{
		if ( desc1.pAttachments[i].srcColorBlendFactor != desc2.pAttachments[i].srcColorBlendFactor) return false;
		if ( desc1.pAttachments[i].dstColorBlendFactor != desc2.pAttachments[i].dstColorBlendFactor) return false;
		if ( desc1.pAttachments[i].colorBlendOp != desc2.pAttachments[i].colorBlendOp) return false;
		if ( desc1.pAttachments[i].srcAlphaBlendFactor != desc2.pAttachments[i].srcAlphaBlendFactor) return false;
		if ( desc1.pAttachments[i].dstAlphaBlendFactor != desc2.pAttachments[i].dstAlphaBlendFactor) return false;
		if ( desc1.pAttachments[i].alphaBlendOp != desc2.pAttachments[i].alphaBlendOp) return false;
		if ( desc1.pAttachments[i].blendEnable != desc2.pAttachments[i].blendEnable) return false;
		if ( desc1.pAttachments[i].colorWriteMask != desc2.pAttachments[i].colorWriteMask) return false;
	}

	return true;
}

bool operator==(const D3D_SAMPLER_DESC &desc1, const D3D_SAMPLER_DESC &desc2)
{
	if( desc1.minFilter != desc2.minFilter) return false;
	if( desc1.magFilter != desc2.magFilter) return false;
	if( desc1.mipmapMode != desc2.mipmapMode) return false;
	if( desc1.addressModeU != desc2.addressModeU) return false;
	if( desc1.addressModeV != desc2.addressModeV) return false;
	if( desc1.addressModeW != desc2.addressModeW) return false;
	if( desc1.mipLodBias != desc2.mipLodBias) return false;
//	Ignore anisotropy since it's set up automatically by the manager
//	if( desc1.anisotropyEnable != desc2.anisotropyEnable) return false;
//	if( desc1.maxAnisotropy != desc2.maxAnisotropy) return false;
	if( desc1.compareEnable != desc2.compareEnable) return false;
	if( desc1.compareOp != desc2.compareOp) return false;
	if( desc1.borderColor != desc2.borderColor) return false;
	if( desc1.minLod != desc2.minLod) return false;
	if( desc1.maxLod != desc2.maxLod) return false;
	if( desc1.unnormalizedCoordinates != desc2.unnormalizedCoordinates) return false;

	return true;
}

u32 GetHash( const D3D_RASTERIZER_DESC &desc )
{
	dxHashHelper	Hash;

	Hash.AddData( &desc.FillMode, sizeof(desc.FillMode) );
	Hash.AddData( &desc.CullMode, sizeof(desc.CullMode) );
	Hash.AddData( &desc.FrontCounterClockwise, sizeof(desc.FrontCounterClockwise) );
	Hash.AddData( &desc.DepthBias, sizeof(desc.DepthBias) );
	Hash.AddData( &desc.DepthBiasClamp, sizeof(desc.DepthBiasClamp) );
	Hash.AddData( &desc.SlopeScaledDepthBias, sizeof(desc.SlopeScaledDepthBias) );
	Hash.AddData( &desc.DepthClipEnable, sizeof(desc.DepthClipEnable) );
	Hash.AddData( &desc.ScissorEnable, sizeof(desc.ScissorEnable) );
	Hash.AddData( &desc.MultisampleEnable, sizeof(desc.MultisampleEnable) );
	Hash.AddData( &desc.AntialiasedLineEnable, sizeof(desc.AntialiasedLineEnable) );
	
	return Hash.GetHash();
}

u32 GetHash( const D3D_DEPTH_STENCIL_DESC &desc )
{
	dxHashHelper	Hash;

	Hash.AddData( &desc.DepthEnable, sizeof(desc.DepthEnable) );
	Hash.AddData( &desc.DepthWriteMask, sizeof(desc.DepthWriteMask) );
	Hash.AddData( &desc.DepthFunc, sizeof(desc.DepthFunc) );
	Hash.AddData( &desc.StencilEnable, sizeof(desc.StencilEnable) );
	Hash.AddData( &desc.StencilReadMask, sizeof(desc.StencilReadMask) );
	Hash.AddData( &desc.StencilWriteMask, sizeof(desc.StencilWriteMask) );

	Hash.AddData( &desc.FrontFace.StencilFailOp, sizeof(desc.FrontFace.StencilFailOp) );
	Hash.AddData( &desc.FrontFace.StencilDepthFailOp, sizeof(desc.FrontFace.StencilDepthFailOp) );
	Hash.AddData( &desc.FrontFace.StencilPassOp, sizeof(desc.FrontFace.StencilPassOp) );
	Hash.AddData( &desc.FrontFace.StencilFunc, sizeof(desc.FrontFace.StencilFunc) );

	Hash.AddData( &desc.BackFace.StencilFailOp, sizeof(desc.BackFace.StencilFailOp) );
	Hash.AddData( &desc.BackFace.StencilDepthFailOp, sizeof(desc.BackFace.StencilDepthFailOp) );
	Hash.AddData( &desc.BackFace.StencilPassOp, sizeof(desc.BackFace.StencilPassOp) );
	Hash.AddData( &desc.BackFace.StencilFunc, sizeof(desc.BackFace.StencilFunc) );

	return Hash.GetHash();
}

u32 GetHash( const D3D_BLEND_DESC &desc )
{
	dxHashHelper	Hash;

	Hash.AddData( &desc.AlphaToCoverageEnable, sizeof(desc.AlphaToCoverageEnable) );
	Hash.AddData( &desc.IndependentBlendEnable, sizeof(desc.IndependentBlendEnable) );

	for ( int i=0; i<8; ++i)
	{
		Hash.AddData( &desc.RenderTarget[i].SrcBlend, sizeof(desc.RenderTarget[i].SrcBlend) );
		Hash.AddData( &desc.RenderTarget[i].DestBlend, sizeof(desc.RenderTarget[i].DestBlend) );
		Hash.AddData( &desc.RenderTarget[i].BlendOp, sizeof(desc.RenderTarget[i].BlendOp) );
		Hash.AddData( &desc.RenderTarget[i].SrcBlendAlpha, sizeof(desc.RenderTarget[i].SrcBlendAlpha) );
		Hash.AddData( &desc.RenderTarget[i].DestBlendAlpha, sizeof(desc.RenderTarget[i].DestBlendAlpha) );
		Hash.AddData( &desc.RenderTarget[i].BlendOpAlpha, sizeof(desc.RenderTarget[i].BlendOpAlpha) );
		Hash.AddData( &desc.RenderTarget[i].BlendEnable, sizeof(desc.RenderTarget[i].BlendEnable) );
		Hash.AddData( &desc.RenderTarget[i].RenderTargetWriteMask, sizeof(desc.RenderTarget[i].RenderTargetWriteMask) );
	}

	return Hash.GetHash();
}

u32 GetHash( const D3D_SAMPLER_DESC &desc )
{
	dxHashHelper	Hash;

	Hash.AddData( &desc.Filter, sizeof(desc.Filter) );
	Hash.AddData( &desc.AddressU, sizeof(desc.AddressU) );
	Hash.AddData( &desc.AddressV, sizeof(desc.AddressV) );
	Hash.AddData( &desc.AddressW, sizeof(desc.AddressW) );
	Hash.AddData( &desc.MipLODBias, sizeof(desc.MipLODBias) );
//	Ignore anisotropy since it's set up automatically by the manager
//	Hash.AddData( &desc.MaxAnisotropy, sizeof(desc.MaxAnisotropy) );
	Hash.AddData( &desc.ComparisonFunc, sizeof(desc.ComparisonFunc) );
	Hash.AddData( &desc.BorderColor[0], sizeof(desc.BorderColor[0]) );
	Hash.AddData( &desc.BorderColor[1], sizeof(desc.BorderColor[1]) );
	Hash.AddData( &desc.BorderColor[2], sizeof(desc.BorderColor[2]) );
	Hash.AddData( &desc.BorderColor[3], sizeof(desc.BorderColor[3]) );
	Hash.AddData( &desc.MinLOD, sizeof(desc.MinLOD) );
	Hash.AddData( &desc.MaxLOD, sizeof(desc.MaxLOD) );

	return Hash.GetHash();
}

void ValidateState(D3D_RASTERIZER_DESC &desc)
{
}

void ValidateState(D3D_DEPTH_STENCIL_DESC &desc)
{
	VERIFY( (desc.DepthEnable==0) || (desc.DepthEnable==1));
	VERIFY( (desc.StencilEnable==0) || (desc.StencilEnable==1));

	if (!desc.DepthEnable)
	{
        desc.DepthWriteMask = D3D_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D_COMPARISON_LESS;
	}

	if (!desc.StencilEnable)
	{
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0xFF;

		desc.FrontFace.StencilFailOp = D3D_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D_COMPARISON_ALWAYS;

		desc.BackFace.StencilFailOp = D3D_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D_STENCIL_OP_KEEP;
		desc.BackFace.StencilPassOp = D3D_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc = D3D_COMPARISON_ALWAYS;
	}
}

#ifdef USE_DX11
void ValidateState(D3D_BLEND_DESC &desc)
{
	BOOL	bBlendEnable = FALSE;

	for ( int i=0; i<8; ++i)
	{
		VERIFY( (desc.RenderTarget[i].BlendEnable==0) || (desc.RenderTarget[i].BlendEnable==1));
		bBlendEnable |= desc.RenderTarget[i].BlendEnable;
	}

	for ( int i=0; i<8; ++i)
	{
		if (!bBlendEnable)
		{
			desc.RenderTarget[i].SrcBlend = D3D_BLEND_ONE;
			desc.RenderTarget[i].DestBlend = D3D_BLEND_ZERO;
			desc.RenderTarget[i].BlendOp = D3D_BLEND_OP_ADD;
			desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_ONE;
			desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_ZERO;
			desc.RenderTarget[i].BlendOpAlpha = D3D_BLEND_OP_ADD;
		}
		else
		{
			switch(desc.RenderTarget[i].SrcBlendAlpha)
			{
			case D3D_BLEND_SRC_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_SRC_ALPHA;
				break;
			case D3D_BLEND_INV_SRC_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_INV_SRC_ALPHA;
				break;
			case D3D_BLEND_DEST_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_DEST_ALPHA;
				break;
			case D3D_BLEND_INV_DEST_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_INV_DEST_ALPHA;
				break;
			case D3D_BLEND_SRC1_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_SRC1_ALPHA;
				break;
			case D3D_BLEND_INV_SRC1_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_INV_SRC1_ALPHA;
					break;
			}

			switch(desc.RenderTarget[i].DestBlendAlpha)
			{
			case D3D_BLEND_SRC_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_SRC_ALPHA;
				break;
			case D3D_BLEND_INV_SRC_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_INV_SRC_ALPHA;
				break;
			case D3D_BLEND_DEST_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_DEST_ALPHA;
				break;
			case D3D_BLEND_INV_DEST_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_INV_DEST_ALPHA;
				break;
			case D3D_BLEND_SRC1_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_SRC1_ALPHA;
				break;
			case D3D_BLEND_INV_SRC1_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_INV_SRC1_ALPHA;
				break;
			}
		}
	}
}
#else
void ValidateState(D3D_BLEND_DESC &desc)
{
	BOOL	bBlendEnable = FALSE;

	for ( int i=0; i<8; ++i)
	{
		VERIFY( (desc.BlendEnable[i]==0) || (desc.BlendEnable[i]==1));
		bBlendEnable |= desc.BlendEnable[i];
	}

	if (!bBlendEnable)
	{
		desc.SrcBlend = D3D_BLEND_ONE;
		desc.DestBlend = D3D_BLEND_ZERO;
		desc.BlendOp = D3D_BLEND_OP_ADD;
		desc.SrcBlendAlpha = D3D_BLEND_ONE;
		desc.DestBlendAlpha = D3D_BLEND_ZERO;
		desc.BlendOpAlpha = D3D_BLEND_OP_ADD;
	}
	else
	{
		switch(desc.SrcBlendAlpha)
		{
		case D3D_BLEND_SRC_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_SRC_ALPHA;
			break;
		case D3D_BLEND_INV_SRC_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_INV_SRC_ALPHA;
			break;
		case D3D_BLEND_DEST_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_DEST_ALPHA;
			break;
		case D3D_BLEND_INV_DEST_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_INV_DEST_ALPHA;
			break;
		case D3D_BLEND_SRC1_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_SRC1_ALPHA;
			break;
		case D3D_BLEND_INV_SRC1_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_INV_SRC1_ALPHA;
				break;
		}

		switch(desc.DestBlendAlpha)
		{
		case D3D_BLEND_SRC_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_SRC_ALPHA;
			break;
		case D3D_BLEND_INV_SRC_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_INV_SRC_ALPHA;
			break;
		case D3D_BLEND_DEST_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_DEST_ALPHA;
			break;
		case D3D_BLEND_INV_DEST_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_INV_DEST_ALPHA;
			break;
		case D3D_BLEND_SRC1_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_SRC1_ALPHA;
			break;
		case D3D_BLEND_INV_SRC1_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_INV_SRC1_ALPHA;
			break;
		}
	}
	
}
#endif

void ValidateState(D3D_SAMPLER_DESC &desc)
{
	if (	(desc.AddressU != D3D_TEXTURE_ADDRESS_BORDER)
		 &&	(desc.AddressV != D3D_TEXTURE_ADDRESS_BORDER)
		 &&	(desc.AddressW != D3D_TEXTURE_ADDRESS_BORDER))
	{
		for (int i=0; i<4; ++i)
		{
			desc.BorderColor[i] = 0.0f;
		}
	}

	if (	(desc.Filter != D3D_FILTER_ANISOTROPIC)
		 && (desc.Filter != D3D_FILTER_COMPARISON_ANISOTROPIC))
	{
		desc.MaxAnisotropy = 1;
	}
}

};