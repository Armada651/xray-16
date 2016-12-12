#ifndef	vkStateManager_included
#define	vkStateManager_included
#pragma once

class vkStateManager
{
public:
	vkStateManager();
	~vkStateManager();

	//	Set all states to default
	void	Reset();
	//	Sends states to DX10 runtime, creates new state objects if nesessary
	void	Apply();

	//	Unmaps all constant bindings.
	void	UnmapConstants();

	//	Set appropriate state. The fastest method. Prefer this code.
	void	SetState(vkState* pState);
	void	SetStencilRef(UINT uiStencilRef);
	void	SetAlphaRef(UINT uiAlphaRef);

	void	BindAlphaRef(R_constant* C);

	//	The slowest (but finer) method
	//	Can create state objects in runtime
	//	These functions accept only DX9 style constants
	//	Don't use these directly. Only via RCache to allow it
	//	to route calls to DX9
	//	TODO: replace u32 with appropriate DX9 enums to avoid confusion
	void	SetStencil(u32 Enable, u32 Func, u32 Ref, u32 Mask, u32 WriteMask, u32 Fail, u32 Pass, u32 ZFail);
	void	SetDepthFunc(u32 Func);
	void	SetDepthEnable(u32 Enable);
	void	SetColorWriteEnable(u32 WriteMask);
	void	SetCullMode(u32 Mode);
// void  SetMultisample( u32 Enable );
   void  SetSampleMask( u32 Mask );

	//void	EnableScissoring(BOOL bEnable = TRUE);

	//	Functions which override value even if new state was set up.
	//	Reset value to current state's value if override is disabled.
	//void	OverrideScissoring(bool bOverride = true, BOOL bValue = TRUE);

private:
	void	ValidateRDesc();
	void	ValidateDSDesc();
	void	ValidateBDesc();

private:
	vkState*					m_pState;

	UINT						m_uiStencilRef;
	UINT						m_uiAlphaRef;

	//	If will need more constants create support class instead
	R_constant*					m_cAlphaRef;

	//	Appropriate state should be applied
	bool						m_bStateNeedApply;

	//	Appropriate state is invalid. 
	//	Get a new state from cache by description before apply.
	bool						m_bRSChanged;
	bool						m_bDSSChanged;
	bool						m_bBSChanged;

	//	Appropriate description is invalid.
	//	Init description before changing.
	bool						m_bRDInvalid;
	bool						m_bDSDInvalid;
	bool						m_bBDInvalid;

	//	Cached state descriptions
	//	Valid only if *Valid flag is set
	D3D_RASTERIZER_DESC		m_RDesc;
	D3D_DEPTH_STENCIL_DESC	m_DSDesc;
	D3D_BLEND_DESC			m_BDesc;

	bool						m_bOverrideScissoring;
	BOOL						m_bOverrideScissoringValue;
   UINT                 m_uiSampleMask;
};

extern	vkStateManager	StateManager;

#endif	//	vkStateManager_included