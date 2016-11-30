#include "stdafx.h"
#include "vkState.h"


#include "vkStateCache.h"

vkState::vkState() : 
	m_Pipeline(0),
	m_uiStencilRef(UINT(-1)),
	m_uiAlphaRef(0)
{
}

vkState::~vkState()
{
	//	m_pRasterizerState is a weak link
	//	m_pDepthStencilState is a weak link
	//	m_pBlendState is a weak link
}

vkState* vkState::Create(SimulatorStates& state_code)
{
	vkState	*pState = new vkState();

	state_code.UpdateState(*pState);

	pState->m_pRasterizerState = RSManager.GetState(state_code);
	pState->m_pDepthStencilState = DSSManager.GetState(state_code);
	pState->m_pBlendState = BSManager.GetState(state_code);
	//ID3DxxDevice::CreateSamplerState

	//	Create samplers here
	{
		InitSamplers( pState->m_VSSamplers, state_code, CTexture::rstVertex);
		InitSamplers( pState->m_PSSamplers, state_code, CTexture::rstPixel);
		InitSamplers( pState->m_GSSamplers, state_code, CTexture::rstGeometry);
#ifdef USE_DX11
		InitSamplers( pState->m_HSSamplers, state_code, CTexture::rstHull);
		InitSamplers( pState->m_DSSamplers, state_code, CTexture::rstDomain);
		InitSamplers( pState->m_CSSamplers, state_code, CTexture::rstCompute);
#endif
	}


	return pState;
}

HRESULT vkState::Apply()
{
	VERIFY(m_Pipeline);
	StateManager.SetRasterizerState(m_pRasterizerState);
	if( m_uiStencilRef != -1 )
		StateManager.SetStencilRef(m_uiStencilRef);
	StateManager.SetAlphaRef(m_uiAlphaRef);

	SSManager.GSApplySamplers(m_GSSamplers);
	SSManager.VSApplySamplers(m_VSSamplers);
	SSManager.PSApplySamplers(m_PSSamplers);
#ifdef USE_DX11
	SSManager.HSApplySamplers(m_HSSamplers);
	SSManager.DSApplySamplers(m_DSSamplers);
	SSManager.CSApplySamplers(m_CSSamplers);
#endif

//	static const FLOAT BlendFactor[4] = {0.000f, 0.000f, 0.000f, 0.000f};
//	static const UINT SampleMask = 0xffffffff;

//	VERIFY(m_pRasterizerState);
//	HW.pDevice->RSSetState(m_pRasterizerState);
//	VERIFY(m_pDepthStencilState);
//	HW.pDevice->OMSetDepthStencilState(m_pDepthStencilState, m_uiStencilRef);
//	VERIFY(m_pBlendState);
//	HW.pDevice->OMSetBlendState(m_pBlendState, BlendFactor, SampleMask);

	return S_OK;
}

void vkState::Release()
{
	vkState	*pState = this;
	xr_delete<vkState> (pState);
}

void vkState::InitSamplers( tSamplerHArray& SamplerArray, SimulatorStates& state_code, int iBaseSamplerIndex)
{
	D3D_SAMPLER_DESC descArray[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
	bool SamplerUsed[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];

	for (int i=0; i<D3D_COMMONSHADER_SAMPLER_SLOT_COUNT; ++i)
	{
		SamplerUsed[i] = false;
		vkStateUtils::ResetDescription(descArray[i]);
	}

	state_code.UpdateDesc(descArray, SamplerUsed, iBaseSamplerIndex);

	int iMaxSampler = D3D_COMMONSHADER_SAMPLER_SLOT_COUNT - 1;
	for ( ;iMaxSampler>-1; --iMaxSampler)
	{
		if (SamplerUsed[iMaxSampler])
			break;
	}

	if (iMaxSampler>-1)
	{
		SamplerArray.reserve(iMaxSampler+1);
		for ( int i=0; i<=iMaxSampler; ++i )
		{
			if (SamplerUsed[i])
				SamplerArray.push_back(SSManager.GetState(descArray[i]));
			else
				SamplerArray.push_back(u32(vkSamplerStateCache::hInvalidHandle));
		}
	}
}