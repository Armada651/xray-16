#ifndef	vkStateCache_included
#define	vkStateCache_included
#pragma once

class vkStateCache
{
	//	Public interface
public:
	vkStateCache();
	~vkStateCache();

	void			ClearStateArray();

	VkPipeline		GetState( SimulatorStates& state_code );
	VkPipeline		GetState( VkGraphicsPipelineCreateInfo& desc );
	//	Can be called on device destruction only!
	//	vkState holds weak links on manager's states and 
	//	won't understand that state was destroyed
	//void	FlushStates();
	//	Private functionality

	//	Private declarations
private:
	struct StateRecord 
	{
		u32						m_crc;
		VkPipeline				m_pState;
		VkGraphicsPipelineCreateInfo m_desc;
	};

private:
	void			CreateState( VkGraphicsPipelineCreateInfo desc, VkPipeline* pState );
	VkPipeline		FindState( const VkGraphicsPipelineCreateInfo& desc, u32 StateCRC );

	//	Private data
private:
	//	This must be cleared on device destroy
	xr_vector<StateRecord>	m_StateArray;
};

extern vkStateCache StateCache;

#include "vkStateCacheImpl.h"

#endif	//	vkStateCache_included