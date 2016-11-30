#ifndef	vkStateCacheImpl_included
#define	vkStateCacheImpl_included
#pragma once
#include "Layers/xrRenderVK/vkStateUtils.h"
#include "Layers/xrRender/Utils/dxHashHelper.h"

using vkStateUtils::operator==;

VkPipeline vkStateCache::GetState( SimulatorStates& state_code )
{
	StateDecs		desc;
	vkStateUtils::ResetDescription(desc.m_RDesc);
	vkStateUtils::ResetDescription(desc.m_DSDesc);
	vkStateUtils::ResetDescription(desc.m_BDesc);
	state_code.UpdateDesc(desc.m_RDesc);
	state_code.UpdateDesc(desc.m_DSDesc);
	state_code.UpdateDesc(desc.m_BDesc);

	return GetState(desc);
}

VkPipeline vkStateCache::GetState( StateDecs& desc )
{
	VkPipeline	pResult;

	vkStateUtils::ValidateState(desc.m_RDesc);
	vkStateUtils::ValidateState(desc.m_DSDesc);
	vkStateUtils::ValidateState(desc.m_BDesc);

	dxHashHelper hash;
	vkStateUtils::GetHash(hash, desc.m_RDesc);
	vkStateUtils::GetHash(hash, desc.m_DSDesc);
	vkStateUtils::GetHash(hash, desc.m_BDesc);
	u32 crc = hash.GetHash();

	pResult = FindState( desc, crc);

	if (!pResult)
	{
		StateRecord rec;
		rec.m_crc = crc;
		CreateState(desc, &rec.m_pState);
		pResult = rec.m_pState;
		m_StateArray.push_back(rec);
	}

	return pResult;
}

VkPipeline vkStateCache::FindState( const StateDecs& desc, u32 StateCRC )
{
    u32 res = 0xffffffff;
	for (u32 i=0; i<m_StateArray.size(); ++i)
	{
		if (m_StateArray[i].m_crc==StateCRC)
		{
			StateDecs	descCandidate = m_StateArray[i].m_desc;
			//if ( !memcmp(&descCandidate, &desc, sizeof(desc)) )
			if (descCandidate.m_RDesc == desc.m_RDesc &&
				descCandidate.m_DSDesc == desc.m_DSDesc &&
				descCandidate.m_BDesc == desc.m_BDesc)
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