#include <stdafx.h>
#include "DXCommonTypes.h"

DXBuffer::DXBuffer()
    : m_buffer()
    , m_memory()
{
}

DXBuffer::~DXBuffer()
{
    vkFreeMemory(HW.device, m_memory, nullptr);
    vkDestroyBuffer(HW.device, m_buffer, nullptr);
}

bool DXBuffer::MemoryTypeFromProperties(VkPhysicalDeviceMemoryProperties properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex)
{
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

VkResult DXBuffer::Create(const void* pData, VkDeviceSize DataSize, VkBufferUsageFlags Usage)
{
    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.usage = Usage;
    buf_info.size = DataSize;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkResult res = vkCreateBuffer(HW.device, &buf_info, NULL, &m_buffer);

    if (res != VK_SUCCESS)
        return res;

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(HW.device, m_buffer, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = NULL;
    alloc_info.memoryTypeIndex = 0;

    alloc_info.allocationSize = mem_reqs.size;
    bool pass = MemoryTypeFromProperties(HW.memoryProperties, mem_reqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &alloc_info.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");

    res = vkAllocateMemory(HW.device, &alloc_info, NULL, &m_memory);

    if (res != VK_SUCCESS)
        return res;

    if (pData)
    {
        void* pMemory;
        res = vkMapMemory(HW.device, m_memory, 0, mem_reqs.size, 0, (void **)&pMemory);

        if (res != VK_SUCCESS)
            return res;

        memcpy(pMemory, pData, DataSize);

        vkUnmapMemory(HW.device, m_memory);
    }

    return vkBindBufferMemory(HW.device, m_buffer, m_memory, 0);
}
