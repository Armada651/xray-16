#include <stdafx.h>
#include "DXCommonTypes.h"

DXBuffer::DXBuffer()
    : m_buffer(VK_NULL_HANDLE)
    , m_memory(VK_NULL_HANDLE)
{
}

DXBuffer::~DXBuffer()
{
    vkFreeMemory(HW.device, m_memory, nullptr);
    vkDestroyBuffer(HW.device, m_buffer, nullptr);
}

VkResult DXBuffer::Create(const void* data, VkDeviceSize dataSize, VkBufferUsageFlags usage)
{
    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.usage = usage;
    buf_info.size = dataSize;
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
    bool pass = HW.GetMemoryType(mem_reqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &alloc_info.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");

    res = vkAllocateMemory(HW.device, &alloc_info, NULL, &m_memory);

    if (res != VK_SUCCESS)
        return res;

    if (data)
    {
        void* pMemory;
        res = vkMapMemory(HW.device, m_memory, 0, mem_reqs.size, 0, (void **)&pMemory);

        if (res != VK_SUCCESS)
            return res;

        memcpy(pMemory, data, dataSize);

        vkUnmapMemory(HW.device, m_memory);
    }

    return vkBindBufferMemory(HW.device, m_buffer, m_memory, 0);
}

DXShader::DXShader()
    : m_shader(VK_NULL_HANDLE)
{
}

DXShader::~DXShader()
{
    vkDestroyShaderModule(HW.device, m_shader, nullptr);
}

VkResult DXShader::Create(const uint32_t* pCode, size_t codeSize)
{
    VkShaderModuleCreateInfo moduleCreateInfo = {};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.codeSize = codeSize;
    moduleCreateInfo.pCode = pCode;
    return vkCreateShaderModule(HW.device, &moduleCreateInfo, NULL, &m_shader);
}

DXTexture::DXTexture()
    : m_image(VK_NULL_HANDLE)
    , m_memory(VK_NULL_HANDLE)
    , m_stagingBuffer(VK_NULL_HANDLE)
    , m_stagingMemory(VK_NULL_HANDLE)
    , m_copyCmd(VK_NULL_HANDLE)
    , m_createInfo()
{
}

DXTexture::~DXTexture()
{
    vkDestroyImage(HW.device, m_image, nullptr);
    vkFreeMemory(HW.device, m_memory, nullptr);
    vkDestroyBuffer(HW.device, m_stagingBuffer, nullptr);
    vkFreeMemory(HW.device, m_stagingMemory, nullptr);
}

VkResult DXTexture::Create(const void* pData, VkDeviceSize dataSize, VkImageCreateInfo createInfo, bool bStaging)
{
    VkResult res = vkCreateImage(HW.device, &createInfo, nullptr, &m_image);

    VkMemoryAllocateInfo memAllocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    VkMemoryRequirements memReqs = {};

    // Get memory requirements for this image 
    // like size and alignment
    vkGetImageMemoryRequirements(HW.device, m_image, &memReqs);
    // Set memory allocation size to required memory size
    memAllocInfo.allocationSize = memReqs.size;

    // Get memory type that can be mapped to host memory
    VERIFY(HW.GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memAllocInfo.memoryTypeIndex));

    // Allocate host memory
    CHK_VK(vkAllocateMemory(HW.device, &memAllocInfo, nullptr, &m_memory));

    // Bind allocated image for use
    CHK_VK(vkBindImageMemory(HW.device, m_image, m_memory, 0));

    if (bStaging)
    {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = dataSize;
        // This buffer is used as a transfer source for the buffer copy
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        CHK_VK(vkCreateBuffer(HW.device, &bufferCreateInfo, nullptr, &m_stagingBuffer));

        // Get memory requirements for the staging buffer (alignment, memory type bits)
        vkGetBufferMemoryRequirements(HW.device, m_stagingBuffer, &memReqs);

        memAllocInfo.allocationSize = memReqs.size;
        // Get memory type index for a host visible buffer
        VERIFY(HW.GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memAllocInfo.memoryTypeIndex));

        CHK_VK(vkAllocateMemory(HW.device, &memAllocInfo, nullptr, &m_stagingMemory));
        CHK_VK(vkBindBufferMemory(HW.device, m_stagingBuffer, m_stagingMemory, 0));

        // Copy texture data into staging buffer
        void* data;
        CHK_VK(vkMapMemory(HW.device, m_stagingMemory, 0, memReqs.size, 0, (void**)&data));
        memcpy(data, pData, dataSize);
        vkUnmapMemory(HW.device, m_stagingMemory);
    }
    else
    {
        // Get sub resource layout
        // Mip map count, array layer, etc.
        VkImageSubresource subRes = {};
        subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        VkSubresourceLayout subResLayout;
        void *data;

        // Get sub resources layout 
        // Includes row pitch, size offsets, etc.
        vkGetImageSubresourceLayout(HW.device, m_image, &subRes, &subResLayout);

        // Map image memory
        CHK_VK(vkMapMemory(HW.device, m_memory, 0, memReqs.size, 0, &data));

        // Copy image data into memory
        memcpy(data, pData, dataSize);

        vkUnmapMemory(HW.device, m_memory);
    }

    // This command buffer should be used by the texture loader to transition image layouts
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = HW.cmdPool;
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(HW.device, &allocInfo, &m_copyCmd);
}
