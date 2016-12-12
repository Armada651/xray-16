#ifndef	vkStateCache_included
#define	vkStateCache_included
#pragma once

class vkStateCache
{
public:
	typedef	struct {
		D3D_RASTERIZER_DESC		m_RDesc;
		D3D_DEPTH_STENCIL_DESC	m_DSDesc;
		D3D_BLEND_DESC			m_BDesc;
	} StateDecs;

	//	Public interface
public:
	vkStateCache();
	~vkStateCache();

	void			Init(const VkGraphicsPipelineCreateInfo& info);
	void			ClearStateArray();

	VkPipeline	GetState(SimulatorStates& state_code);
	VkPipeline	GetState(StateDecs& desc);
	//	Can be called on device destruction only!
	//	dx10State holds weak links on manager's states and 
	//	won't understand that state was destroyed
	//void	FlushStates();
	//	Private functionality

	//	Private declarations
private:
	struct StateRecord
	{
		u32						m_crc;
		StateDecs				m_desc;
		VkPipeline				m_pState;
	};

private:
	void			CreateState(StateDecs desc, VkPipeline* pState);
	VkPipeline		FindState(const StateDecs& desc, u32 StateCRC);

	//	Private data
private:
	//	This must be cleared on device destroy
	xr_vector<StateRecord>			m_StateArray;
	VkPipeline						m_Pipeline;
	VkGraphicsPipelineCreateInfo	m_PipelineInfo;
};

#include "vkStateCacheImpl.h"

#endif	//	vkStateCache_included