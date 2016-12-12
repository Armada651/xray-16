#include "stdafx.h"
#include "vkStateManager.h"
#include "Layers/xrRenderVK/vkStateUtils.h"

vkStateManager	StateManager;

//	DX10: TODO: Implement alpha referense control

vkStateManager::vkStateManager()
{
	//	If vkStateManager would ever own any object
	//	implement correct state manager
	Reset();
}

vkStateManager::~vkStateManager()
{
	//	Don't own any object so no release is needed
}

//	Set all states to default
void vkStateManager::Reset()
{
	UnmapConstants();

	m_pState = nullptr;

	m_uiStencilRef = 0;
	m_uiAlphaRef = 0;

	m_bRSNeedApply = true;
	m_bDSSNeedApply = true;
	m_bBSNeedApply = true;

	m_bRSChanged = false;
	m_bDSSChanged = false;
	m_bBSChanged = false;

	m_bRDInvalid = false;
	m_bDSDInvalid = false;
	m_bBDInvalid = false;

	vkStateUtils::ResetDescription(m_RDesc);
	vkStateUtils::ResetDescription(m_DSDesc);
	vkStateUtils::ResetDescription(m_BDesc);

	m_bOverrideScissoring = false;
	m_bOverrideScissoringValue = FALSE;
   m_uiSampleMask = 0xffffffff;
}

void vkStateManager::UnmapConstants()
{
	m_cAlphaRef = 0;
}

void vkStateManager::SetState(vkState* pState)
{
	m_bRSChanged = false;
	m_bDSSChanged = false;
	m_bBSChanged = false;

	m_bRDInvalid = true;
	m_bDSDInvalid = true;
	m_bBDInvalid = true;

	if (pState != m_pState)
	{
		m_pState = pState;
		m_bStateNeedApply = true;
	}
}

void vkStateManager::SetStencilRef(UINT uiStencilRef)
{
	if ( m_uiStencilRef != uiStencilRef)
	{
		m_uiStencilRef = uiStencilRef;
		m_bDSSChanged = true;
	}
}

void vkStateManager::SetAlphaRef(UINT uiAlphaRef)
{
	if ( m_uiAlphaRef != uiAlphaRef)
	{
		m_uiAlphaRef = uiAlphaRef;
		if (m_cAlphaRef) RCache.set_c( m_cAlphaRef, (float)m_uiAlphaRef/255.0f );
	}
}

void vkStateManager::BindAlphaRef(R_constant* C)
{
	m_cAlphaRef = C;
	if (m_cAlphaRef) RCache.set_c( m_cAlphaRef, (float)m_uiAlphaRef/255.0f );
}

void vkStateManager::ValidateRDesc()
{
	if (m_bRDInvalid)
	{
		if (m_pState)
			m_pState->GetDesc(&m_RDesc);
		else
			vkStateUtils::ResetDescription(m_RDesc);

		m_bRDInvalid = false;
	}
}

void vkStateManager::ValidateDSDesc()
{
	if (m_bDSDInvalid)
	{
		if (m_pState)
			m_pState->GetDesc(&m_DSDesc);
		else
			vkStateUtils::ResetDescription(m_DSDesc);

		m_bDSDInvalid = false;
	}
}

void vkStateManager::ValidateBDesc()
{
	if (m_bBDInvalid)
	{
		if (m_pState)
			m_pState->GetDesc(&m_BDesc);
		else
			vkStateUtils::ResetDescription(m_BDesc);

		m_bBDInvalid = false;
	}
}

//	Sends states to DX10 runtime, creates new state objects if nessessary
void vkStateManager::Apply()
{
	//	Apply rasterizer state
	/*if ( m_bRSNeedApply || m_bRSChanged )
	{
		if (m_bRSChanged)
		{
			m_pRState = RSManager.GetState(m_RDesc);
			m_bRSChanged = false;
		}

		HW.pContext->RSSetState(m_pRState);
		m_bRSNeedApply = false;
	}

	//	Apply depth stencil state
	if ( m_bDSSNeedApply || m_bDSSChanged )
	{
		if (m_bDSSChanged)
		{
			m_pDepthStencilState = DSSManager.GetState(m_DSDesc);
			m_bDSSChanged = false;
		}

		HW.pContext->OMSetDepthStencilState(m_pDepthStencilState, m_uiStencilRef);
		m_bDSSNeedApply = false;
	}

	//	Apply blend state
	if ( m_bBSNeedApply || m_bBSChanged )
	{
		if (m_bBSChanged)
		{
			m_pBlendState = BSManager.GetState(m_BDesc);
			m_bBSChanged = false;
		}

		static const FLOAT BlendFactor[4] = {0.000f, 0.000f, 0.000f, 0.000f};

		HW.pContext->OMSetBlendState(m_pBlendState, BlendFactor, m_uiSampleMask);
		m_bBSNeedApply = false;
	}*/
}

void vkStateManager::SetStencil(u32 Enable, u32 Func, u32 Ref, u32 Mask, u32 WriteMask, u32 Fail, u32 Pass, u32 ZFail)
{
	ValidateDSDesc();

	// Simple filter
	//if (stencil_enable		!= _enable)		{ stencil_enable=_enable;		CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		_enable				)); }
	VkBool32	BEnable = (VkBool32)Enable;
	if (m_DSDesc.stencilTestEnable!=BEnable)
	{
		m_bDSSChanged = true;
		m_DSDesc.stencilTestEnable = BEnable;
	}
	
	if (!m_DSDesc.stencilTestEnable)	return;

	//if (stencil_func		!= _func)		{ stencil_func=_func;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFUNC,		_func				)); }
	D3D_COMPARISON_FUNC	SFunc = 
		vkStateUtils::ConvertCmpFunction(D3DCMPFUNC(Func));

	if ((m_DSDesc.front.compareOp!=SFunc)
		|| (m_DSDesc.back.compareOp!=SFunc))
	{
		m_bDSSChanged = true;
		m_DSDesc.front.compareOp = SFunc;
		m_DSDesc.back.compareOp = SFunc;
	}

	//if (stencil_ref			!= _ref)		{ stencil_ref=_ref;				CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,			_ref				)); }
	SetStencilRef(Ref);

	//if (stencil_mask		!= _mask)		{ stencil_mask=_mask;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILMASK,		_mask				)); }
	if( m_DSDesc.front.compareMask != Mask ||
		m_DSDesc.back.compareMask != Mask)
	{
		m_bDSSChanged = true;
		m_DSDesc.front.compareMask = Mask;
		m_DSDesc.back.compareMask = Mask;
	}

	//if (stencil_writemask	!= _writemask)	{ stencil_writemask=_writemask;	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILWRITEMASK,	_writemask			)); }
	if( m_DSDesc.front.writeMask != WriteMask ||
		m_DSDesc.back.writeMask != WriteMask)
	{
		m_bDSSChanged = true;
		m_DSDesc.front.writeMask = WriteMask;
		m_DSDesc.back.writeMask = WriteMask;
	}

	//if (stencil_fail		!= _fail)		{ stencil_fail=_fail;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILFAIL,		_fail				)); }
	D3D_STENCIL_OP	SOp = 
		vkStateUtils::ConvertStencilOp(D3DSTENCILOP(Fail));
	if ((m_DSDesc.front.failOp!=SOp)
		|| (m_DSDesc.back.failOp!=SOp))
	{
		m_bDSSChanged = true;
		m_DSDesc.front.failOp = SOp;
		m_DSDesc.back.failOp = SOp;
	}

	//if (stencil_pass		!= _pass)		{ stencil_pass=_pass;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILPASS,		_pass				)); }
	SOp = vkStateUtils::ConvertStencilOp(D3DSTENCILOP(Pass));
	if ((m_DSDesc.front.passOp!=SOp)
		|| (m_DSDesc.back.passOp!=SOp))
	{
		m_bDSSChanged = true;
		m_DSDesc.front.passOp = SOp;
		m_DSDesc.back.passOp = SOp;
	}

	//if (stencil_zfail		!= _zfail)		{ stencil_zfail=_zfail;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILZFAIL,		_zfail				)); }
	SOp = vkStateUtils::ConvertStencilOp(D3DSTENCILOP(ZFail));
	if ((m_DSDesc.front.depthFailOp!=SOp)
		|| (m_DSDesc.back.depthFailOp!=SOp))
	{
		m_bDSSChanged = true;
		m_DSDesc.front.depthFailOp = SOp;
		m_DSDesc.back.depthFailOp = SOp;
	}
}


void vkStateManager::SetDepthFunc(u32 Func)
{
	ValidateDSDesc();

	//if (z_func!=_func)
	//{
	//	z_func = _func;
	//	CHK_DX(HW.pDevice->SetRenderState( D3DRS_ZFUNC, _func));
	//}

	D3D_COMPARISON_FUNC	DFunc = 
		vkStateUtils::ConvertCmpFunction(D3DCMPFUNC(Func));
	if (m_DSDesc.depthCompareOp!=DFunc)
	{
		m_bDSSChanged = true;
		m_DSDesc.depthCompareOp = DFunc;
	}
}


void vkStateManager::SetDepthEnable(u32 Enable)
{
	ValidateDSDesc();

	//if (z_enable != _enable)
	//{ 
	//	z_enable=_enable;
	//	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_ZENABLE, _enable )); 
	//}

	VkBool32	BEnable = (VkBool32)Enable;
	if (m_DSDesc.depthTestEnable!=BEnable)
	{
		m_bDSSChanged = true;
		m_DSDesc.depthTestEnable = BEnable;
	}
}

void vkStateManager::SetColorWriteEnable(u32 WriteMask)
{
	ValidateBDesc();

	//if (colorwrite_mask		!= _mask)		{ 
	//	colorwrite_mask=_mask;		
	//	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE,	_mask	));	
	//	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE1,	_mask	));	
	//	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE2,	_mask	));	
	//	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_COLORWRITEENABLE3,	_mask	));	
	//}

	UINT8	WMask = (UINT8)WriteMask;

	bool	bNeedUpdate = false;
	for (int i=0; i<4; ++i)
	{
		if (m_BDesc.RenderTarget[i].colorWriteMask !=WMask)
			bNeedUpdate = true;
	}

	if (bNeedUpdate)
	{
		m_bBSChanged = true;
		for (int i=0; i<4; ++i)
			m_BDesc.RenderTarget[i].colorWriteMask = WMask;
	}
}

void vkStateManager::SetSampleMask( u32 SampleMask )
{
   if( m_uiSampleMask != SampleMask )
   {
		// TODO: VK: Implement MSAA support.
		m_uiSampleMask = SampleMask;
		m_bBSChanged = true;
   }
}

void vkStateManager::SetCullMode(u32 Mode)
{
	ValidateRDesc();
	//if (cull_mode		!= _mode)		{ cull_mode = _mode;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			_mode				)); }

	D3D_CULL_MODE	CMode = 
		vkStateUtils::ConvertCullMode((D3DCULL)Mode);
	if (m_RDesc.cullMode!=CMode)
	{
		m_bRSChanged = true;
		m_RDesc.cullMode = CMode;
	}
}
