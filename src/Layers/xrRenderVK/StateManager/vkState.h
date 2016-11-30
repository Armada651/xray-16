#ifndef	vkState_included
#define	vkState_included
#pragma once

class SimulatorStates;

#include "vkSamplerStateCache.h"

class vkState
{
//	Public interface
public:
	vkState();	//	These have to be private bu new/xr_delete don't support this
	~vkState();

	static vkState* Create( SimulatorStates& state_code );

	//	DX9 unified interface
	HRESULT Apply();
	void	Release();

	//	DX10 specific
	void	UpdateStencilRef( UINT Ref ) { m_uiStencilRef = Ref; }
	void	UpdateAlphaRef( UINT Ref ) { m_uiAlphaRef = Ref; }

//	User restricted interface
private:
	typedef	vkSamplerStateCache::HArray tSamplerHArray;

private:
	static void InitSamplers( tSamplerHArray& SamplerArray, SimulatorStates& state_code, int iBaseSamplerIndex);

private:
	//	All states are supposed to live along all application lifetime
	VkPipeline m_Pipeline;	//  The vkPipeline contains all states

	tSamplerHArray				m_VSSamplers;
	tSamplerHArray				m_PSSamplers;
	tSamplerHArray				m_GSSamplers;
#ifdef USE_DX11
	tSamplerHArray				m_CSSamplers;
	tSamplerHArray				m_HSSamplers;
	tSamplerHArray				m_DSSamplers;
#endif

	UINT						m_uiStencilRef;
	UINT						m_uiAlphaRef;

//	Private data
private:
};

#endif	//	vkState_included