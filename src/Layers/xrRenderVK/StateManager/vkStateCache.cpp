#include "stdafx.h"
#include "vkStateCache.h"

vkStateCache::vkStateCache(const VkGraphicsPipelineCreateInfo& info)
	: m_PipelineInfo(info)
{
	static const int iMasRSStates = 10;
	m_StateArray.reserve(iMasRSStates);

	m_PipelineInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
	CHK_VK(vkCreateGraphicsPipelines(HW.device, VK_NULL_HANDLE, 1, &m_PipelineInfo, NULL, &m_Pipeline));
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

void vkStateCache::CreateState(StateDecs desc, VkPipeline* pState)
{
	m_PipelineInfo.pRasterizationState = &desc.m_RDesc;
	m_PipelineInfo.pDepthStencilState = &desc.m_DSDesc;
	m_PipelineInfo.pColorBlendState = &desc.m_BDesc.Blend;
	m_PipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;

	// TODO: VK: Use a state cache
	CHK_VK(vkCreateGraphicsPipelines(HW.device, VK_NULL_HANDLE, 1, &m_PipelineInfo, NULL, pState));

	//	TODO: VK: Remove this.
#ifdef	DEBUG
	Msg("VkPipeline #%d created.", m_StateArray.size());
#endif	//	DEBUG
}
