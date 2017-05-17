#include "stdafx.h"
#include "vkConstantBuffer.h"
#include "vkBufferUtils.h"

#include <Types.h>

vkConstantBuffer::~vkConstantBuffer()
{
    RImplementation.Resources->_DeleteConstantBuffer(this);
    //	Flush();
    _RELEASE(m_pBuffer);
    delete m_Table;
    xr_free(m_pBufferData);
}

vkConstantBuffer::vkConstantBuffer(const glslang::TType* pTable, u32 size) : m_bChanged(true)
{
    const glslang::TTypeList* Desc = pTable->getStruct();

    VERIFY(Desc);

    m_strBufferName._set(pTable->getTypeName().c_str());
    m_eBufferType = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    m_uiBufferSize = size;

    //	Fill member list with variable descriptions
    m_Table = pTable->clone();

    CHK_VK(vkBufferUtils::CreateConstantBuffer(&m_pBuffer, size));
    VERIFY(m_pBuffer);
    m_pBufferData = xr_malloc(size);
    VERIFY(m_pBufferData);
}

bool vkConstantBuffer::Similar(vkConstantBuffer& _in)
{
    return m_Table->sameStructType(*_in.m_Table);
}

void vkConstantBuffer::Flush()
{
    if (m_bChanged)
    {
        void* pData;
        vkMapMemory(HW.device, m_pBuffer->m_memory, 0, m_uiBufferSize, 0, (void **)&pData);
        VERIFY(pData);
        VERIFY(m_pBufferData);
        memcpy(pData, m_pBufferData, m_uiBufferSize);
        vkUnmapMemory(HW.device, m_pBuffer->m_memory);
        m_bChanged = false;
    }
}
