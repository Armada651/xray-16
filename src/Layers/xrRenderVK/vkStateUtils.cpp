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
	desc.Blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	desc.Blend.attachmentCount = 8;
	desc.Blend.pAttachments = desc.RenderTarget;

	for (int i = 0; i<8; ++i)
	{
		desc.RenderTarget[i].srcColorBlendFactor = D3D_BLEND_ONE;
		desc.RenderTarget[i].dstColorBlendFactor = D3D_BLEND_ZERO;
		desc.RenderTarget[i].colorBlendOp = D3D_BLEND_OP_ADD;
		desc.RenderTarget[i].srcAlphaBlendFactor = D3D_BLEND_ONE;
		desc.RenderTarget[i].dstAlphaBlendFactor = D3D_BLEND_ZERO;
		desc.RenderTarget[i].alphaBlendOp = D3D_BLEND_OP_ADD;
		desc.RenderTarget[i].blendEnable = VK_FALSE;
		desc.RenderTarget[i].colorWriteMask = D3D_COLOR_WRITE_ENABLE_ALL;
	}
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
	// Ignore blend constants, since we never use them
	if ( desc1.Blend.logicOpEnable != desc2.Blend.logicOpEnable) return false;
	if ( desc1.Blend.logicOp != desc2.Blend.logicOp) return false;
	if ( desc1.Blend.attachmentCount != desc2.Blend.attachmentCount) return false;

	for (int i = 0; i<4/*8*//*It's quick fix for dx11 port because of mispatch of desc*/; ++i)
	{
		if ( desc1.RenderTarget[i].srcColorBlendFactor != desc2.RenderTarget[i].srcColorBlendFactor) return false;
		if ( desc1.RenderTarget[i].dstColorBlendFactor != desc2.RenderTarget[i].dstColorBlendFactor) return false;
		if ( desc1.RenderTarget[i].colorBlendOp != desc2.RenderTarget[i].colorBlendOp) return false;
		if ( desc1.RenderTarget[i].srcAlphaBlendFactor != desc2.RenderTarget[i].srcAlphaBlendFactor) return false;
		if ( desc1.RenderTarget[i].dstAlphaBlendFactor != desc2.RenderTarget[i].dstAlphaBlendFactor) return false;
		if ( desc1.RenderTarget[i].alphaBlendOp != desc2.RenderTarget[i].alphaBlendOp) return false;
		if ( desc1.RenderTarget[i].blendEnable != desc2.RenderTarget[i].blendEnable) return false;
		if ( desc1.RenderTarget[i].colorWriteMask != desc2.RenderTarget[i].colorWriteMask) return false;
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

void GetHash( dxHashHelper &Hash, const D3D_RASTERIZER_DESC &desc )
{
	Hash.AddData( &desc.polygonMode, sizeof(desc.polygonMode) );
	Hash.AddData( &desc.cullMode, sizeof(desc.cullMode) );
	Hash.AddData( &desc.frontFace, sizeof(desc.frontFace) );
	Hash.AddData( &desc.depthBiasEnable, sizeof(desc.depthBiasEnable) );
	Hash.AddData( &desc.depthBiasConstantFactor, sizeof(desc.depthBiasConstantFactor) );
	Hash.AddData( &desc.depthBiasClamp, sizeof(desc.depthBiasClamp) );
	Hash.AddData( &desc.depthBiasSlopeFactor, sizeof(desc.depthBiasSlopeFactor) );
	Hash.AddData( &desc.lineWidth, sizeof(desc.lineWidth) );
	Hash.AddData( &desc.depthClampEnable, sizeof(desc.depthClampEnable) );
	Hash.AddData( &desc.rasterizerDiscardEnable, sizeof(desc.rasterizerDiscardEnable) );
}

void GetHash( dxHashHelper &Hash, const D3D_DEPTH_STENCIL_DESC &desc )
{
	Hash.AddData( &desc.depthTestEnable, sizeof(desc.depthTestEnable) );
	Hash.AddData( &desc.depthWriteEnable, sizeof(desc.depthWriteEnable) );
	Hash.AddData( &desc.depthCompareOp, sizeof(desc.depthCompareOp) );
	Hash.AddData( &desc.depthBoundsTestEnable, sizeof(desc.depthBoundsTestEnable) );
	Hash.AddData( &desc.stencilTestEnable, sizeof(desc.stencilTestEnable) );
	
	Hash.AddData( &desc.front.compareMask, sizeof(desc.front.compareMask) );
	Hash.AddData( &desc.front.writeMask, sizeof(desc.front.writeMask) );
	Hash.AddData( &desc.front.failOp, sizeof(desc.front.failOp) );
	Hash.AddData( &desc.front.depthFailOp, sizeof(desc.front.depthFailOp) );
	Hash.AddData( &desc.front.passOp, sizeof(desc.front.passOp) );
	Hash.AddData( &desc.front.compareOp, sizeof(desc.front.compareOp) );
	
	Hash.AddData( &desc.back.compareMask, sizeof(desc.back.compareMask) );
	Hash.AddData( &desc.back.writeMask, sizeof(desc.back.writeMask) );
	Hash.AddData( &desc.back.failOp, sizeof(desc.back.failOp) );
	Hash.AddData( &desc.back.depthFailOp, sizeof(desc.back.depthFailOp) );
	Hash.AddData( &desc.back.passOp, sizeof(desc.back.passOp) );
	Hash.AddData( &desc.back.compareOp, sizeof(desc.back.compareOp) );
}

void GetHash( dxHashHelper &Hash, const D3D_BLEND_DESC &desc )
{
	// Ignore blend constants, since we never use them
	Hash.AddData( &desc.Blend.attachmentCount, sizeof(desc.Blend.attachmentCount) );

	for ( int i=0; i<8; ++i)
	{
		Hash.AddData( &desc.RenderTarget[i].srcColorBlendFactor, sizeof(desc.RenderTarget[i].srcColorBlendFactor) );
		Hash.AddData( &desc.RenderTarget[i].dstColorBlendFactor, sizeof(desc.RenderTarget[i].dstColorBlendFactor) );
		Hash.AddData( &desc.RenderTarget[i].colorBlendOp, sizeof(desc.RenderTarget[i].colorBlendOp) );
		Hash.AddData( &desc.RenderTarget[i].srcAlphaBlendFactor, sizeof(desc.RenderTarget[i].srcAlphaBlendFactor) );
		Hash.AddData( &desc.RenderTarget[i].dstAlphaBlendFactor, sizeof(desc.RenderTarget[i].dstAlphaBlendFactor) );
		Hash.AddData( &desc.RenderTarget[i].alphaBlendOp, sizeof(desc.RenderTarget[i].alphaBlendOp) );
		Hash.AddData( &desc.RenderTarget[i].blendEnable, sizeof(desc.RenderTarget[i].blendEnable) );
		Hash.AddData( &desc.RenderTarget[i].colorWriteMask, sizeof(desc.RenderTarget[i].colorWriteMask) );
	}
}

void GetHash( dxHashHelper &Hash, const D3D_SAMPLER_DESC &desc )
{
	Hash.AddData( &desc.minFilter, sizeof(desc.minFilter) );
	Hash.AddData( &desc.magFilter, sizeof(desc.magFilter) );
	Hash.AddData( &desc.mipmapMode, sizeof(desc.mipmapMode) );
	Hash.AddData( &desc.addressModeU, sizeof(desc.addressModeU) );
	Hash.AddData( &desc.addressModeV, sizeof(desc.addressModeV) );
	Hash.AddData( &desc.addressModeW, sizeof(desc.addressModeW) );
	Hash.AddData( &desc.mipLodBias, sizeof(desc.mipLodBias) );
//	Ignore anisotropy since it's set up automatically by the manager
//	Hash.AddData( &desc.anisotropyEnable, sizeof(desc.anisotropyEnable) );
//	Hash.AddData( &desc.maxAnisotropy, sizeof(desc.maxAnisotropy) );
	Hash.AddData( &desc.compareEnable, sizeof(desc.compareEnable) );
	Hash.AddData( &desc.compareOp, sizeof(desc.compareOp) );
	Hash.AddData( &desc.borderColor, sizeof(desc.borderColor) );
	Hash.AddData( &desc.minLod, sizeof(desc.minLod) );
	Hash.AddData( &desc.maxLod, sizeof(desc.maxLod) );
	Hash.AddData( &desc.unnormalizedCoordinates, sizeof(desc.unnormalizedCoordinates) );
}

void ValidateState(D3D_RASTERIZER_DESC &desc)
{
}

void ValidateState(D3D_DEPTH_STENCIL_DESC &desc)
{
	VERIFY( (desc.depthTestEnable==0) || (desc.depthTestEnable ==1));
	VERIFY( (desc.stencilTestEnable==0) || (desc.stencilTestEnable ==1));

	if (!desc.depthTestEnable)
	{
        desc.depthWriteEnable = VK_TRUE;
		desc.depthCompareOp = D3D_COMPARISON_LESS;
	}

	if (!desc.stencilTestEnable)
	{
		desc.front.compareMask = desc.back.compareMask = 0xFF;
		desc.front.writeMask = desc.back.writeMask = 0xFF;

		desc.front.failOp = D3D_STENCIL_OP_KEEP;
		desc.front.depthFailOp = D3D_STENCIL_OP_KEEP;
		desc.front.passOp = D3D_STENCIL_OP_KEEP;
		desc.front.compareOp = D3D_COMPARISON_ALWAYS;

		desc.back.failOp = D3D_STENCIL_OP_KEEP;
		desc.back.depthFailOp = D3D_STENCIL_OP_KEEP;
		desc.back.passOp = D3D_STENCIL_OP_KEEP;
		desc.back.compareOp = D3D_COMPARISON_ALWAYS;
	}
}

void ValidateState(D3D_BLEND_DESC &desc)
{
	BOOL	bBlendEnable = FALSE;

	for ( int i=0; i<8; ++i)
	{
		VERIFY( (desc.RenderTarget[i].blendEnable==0) || (desc.RenderTarget[i].blendEnable ==1));
		bBlendEnable |= desc.RenderTarget[i].blendEnable;
	}

	for ( int i=0; i<8; ++i)
	{
		if (!bBlendEnable)
		{
			desc.RenderTarget[i].srcColorBlendFactor = D3D_BLEND_ONE;
			desc.RenderTarget[i].dstColorBlendFactor = D3D_BLEND_ZERO;
			desc.RenderTarget[i].colorBlendOp = D3D_BLEND_OP_ADD;
			desc.RenderTarget[i].srcAlphaBlendFactor = D3D_BLEND_ONE;
			desc.RenderTarget[i].dstAlphaBlendFactor = D3D_BLEND_ZERO;
			desc.RenderTarget[i].alphaBlendOp = D3D_BLEND_OP_ADD;
		}
		else
		{
			switch(desc.RenderTarget[i].srcAlphaBlendFactor)
			{
			case D3D_BLEND_SRC_COLOR:
				desc.RenderTarget[i].srcAlphaBlendFactor = D3D_BLEND_SRC_ALPHA;
				break;
			case D3D_BLEND_INV_SRC_COLOR:
				desc.RenderTarget[i].srcAlphaBlendFactor = D3D_BLEND_INV_SRC_ALPHA;
				break;
			case D3D_BLEND_DEST_COLOR:
				desc.RenderTarget[i].srcAlphaBlendFactor = D3D_BLEND_DEST_ALPHA;
				break;
			case D3D_BLEND_INV_DEST_COLOR:
				desc.RenderTarget[i].srcAlphaBlendFactor = D3D_BLEND_INV_DEST_ALPHA;
				break;
			case D3D_BLEND_SRC1_COLOR:
				desc.RenderTarget[i].srcAlphaBlendFactor = D3D_BLEND_SRC1_ALPHA;
				break;
			case D3D_BLEND_INV_SRC1_COLOR:
				desc.RenderTarget[i].srcAlphaBlendFactor = D3D_BLEND_INV_SRC1_ALPHA;
					break;
			}

			switch(desc.RenderTarget[i].dstAlphaBlendFactor)
			{
			case D3D_BLEND_SRC_COLOR:
				desc.RenderTarget[i].dstAlphaBlendFactor = D3D_BLEND_SRC_ALPHA;
				break;
			case D3D_BLEND_INV_SRC_COLOR:
				desc.RenderTarget[i].dstAlphaBlendFactor = D3D_BLEND_INV_SRC_ALPHA;
				break;
			case D3D_BLEND_DEST_COLOR:
				desc.RenderTarget[i].dstAlphaBlendFactor = D3D_BLEND_DEST_ALPHA;
				break;
			case D3D_BLEND_INV_DEST_COLOR:
				desc.RenderTarget[i].dstAlphaBlendFactor = D3D_BLEND_INV_DEST_ALPHA;
				break;
			case D3D_BLEND_SRC1_COLOR:
				desc.RenderTarget[i].dstAlphaBlendFactor = D3D_BLEND_SRC1_ALPHA;
				break;
			case D3D_BLEND_INV_SRC1_COLOR:
				desc.RenderTarget[i].dstAlphaBlendFactor = D3D_BLEND_INV_SRC1_ALPHA;
				break;
			}
		}
	}
}

void ValidateState(D3D_SAMPLER_DESC &desc)
{
	if (	(desc.addressModeU != D3D_TEXTURE_ADDRESS_BORDER)
		 &&	(desc.addressModeV != D3D_TEXTURE_ADDRESS_BORDER)
		 &&	(desc.addressModeW != D3D_TEXTURE_ADDRESS_BORDER))
	{
		desc.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
	}

	if (!desc.anisotropyEnable)
	{
		desc.maxAnisotropy = 1;
	}
}

};
