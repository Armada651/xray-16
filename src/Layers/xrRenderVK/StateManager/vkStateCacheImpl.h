#ifndef	vkStateCacheImpl_included
#define	vkStateCacheImpl_included
#pragma once
#include "Layers/xrRenderVK/vkStateUtils.h"

using vkStateUtils::operator==;

/*VkPipeline vkStateCache::GetState( SimulatorStates& state_code )
{
	VkGraphicsPipelineCreateInfo		desc;
	vkStateUtils::ResetDescription(desc);
	state_code.UpdateDesc(desc);

	return GetState(desc);
}*/

VkPipeline vkStateCache::GetState( VkGraphicsPipelineCreateInfo& desc )
{
	VkPipeline	pResult;

	vkStateUtils::ValidateState(desc);

	u32 crc = vkStateUtils::GetHash(desc);

	pResult = FindState( desc, crc);

	if (!pResult)
	{
		StateRecord rec;
		rec.m_crc = crc;
		rec.m_desc = desc;
		CreateState(desc, &rec.m_pState);
		pResult = rec.m_pState;
		m_StateArray.push_back(rec);
	}

	return pResult;
}

VkPipeline vkStateCache::FindState( const VkGraphicsPipelineCreateInfo& desc, u32 StateCRC )
{
    u32 res = 0xffffffff;
	for (u32 i=0; i<m_StateArray.size(); ++i)
	{
		if (m_StateArray[i].m_crc==StateCRC)
		{
			VkGraphicsPipelineCreateInfo	descCandidate = m_StateArray[i].m_desc;
			//if ( !memcmp(&descCandidate, &desc, sizeof(desc)) )
			if (descCandidate==desc)
				//break;
			//	TEST
			{
                res = i;
				break;
			}
			//else
			//{
			//	VERIFY(0);
			//}
		}
	}

	if (res!=0xffffffff)
		return m_StateArray[res].m_pState;
	else
		return NULL;
    /*
	if (i!=m_StateArray.size())
		return m_StateArray[i].m_pState;
	else
		return NULL;
        */
}

#endif	//	vkStateCacheImpl_included