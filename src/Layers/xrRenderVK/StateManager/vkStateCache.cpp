#include "stdafx.h"
#include "vkStateCache.h"

vkStateCache		StateCache;

vkStateCache::vkStateCache()
{
	static const int iMasRSStates = 10;
	m_StateArray.reserve(iMasRSStates);
}

vkStateCache::~vkStateCache()
{
	ClearStateArray();
//	VERIFY(m_StateArray.empty());
}

/*
template <class IDeviceState, class StateDecs>
void 
vkStateCache<IDeviceState, StateDecs>
::FlushStates()
{
	ClearStateArray();
}
*/

void vkStateCache::ClearStateArray()
{
	for (u32 i=0; i<m_StateArray.size(); ++i)
	{
		vkDestroyPipeline(HW.device, m_StateArray[i].m_pState, NULL);
	}

	m_StateArray.clear_not_free();
}

void vkStateCache::CreateState(VkGraphicsPipelineCreateInfo desc, VkPipeline* pState)
{
	// TODO: VK: Use a state cache and derivative pipelines.
	CHK_VK(vkCreateGraphicsPipelines(HW.device, VK_NULL_HANDLE, 1, &desc, NULL, pState));

	//	TODO: VK: Remove this.
#ifdef	DEBUG
	Msg("VkPipeline #%d created.", m_StateArray.size());
#endif	//	DEBUG
}
