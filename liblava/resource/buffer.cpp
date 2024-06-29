/**
 * @file         liblava/resource/buffer.cpp
 * @brief        Vulkan buffer
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/resource/buffer.hpp"
#include "liblava/util/log.hpp"

namespace lava {

//-----------------------------------------------------------------------------
bool buffer::create(device::ptr dev,
                    void const* data,
                    size_t size,
                    VkBufferUsageFlags usage,
                    bool mapped,
                    VmaMemoryUsage memory_usage,
                    VkSharingMode sharing_mode,
                    std::vector<ui32> const& shared_queue_family_indices,
                    i32 alignment) {
    m_device = dev;

    VkBufferCreateInfo buffer_info{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = sharing_mode,
        .queueFamilyIndexCount = to_ui32(shared_queue_family_indices.size()),
        .pQueueFamilyIndices = shared_queue_family_indices.data()};

    VmaAllocationCreateFlags const alloc_flags = mapped
                                                     ? VMA_ALLOCATION_CREATE_MAPPED_BIT
                                                     : 0;

    VmaAllocationCreateInfo alloc_info{
        .flags = alloc_flags,
        .usage = memory_usage,
    };

    if (alignment >= 0) {
        if (failed(vmaCreateBufferWithAlignment(m_device->alloc(),
                                                &buffer_info,
                                                &alloc_info,
                                                alignment,
                                                &m_vk_buffer,
                                                &m_allocation,
                                                &m_allocation_info))) {
            logger()->error("create buffer with alignment");
            return false;
        }
    } else {
        if (failed(vmaCreateBuffer(m_device->alloc(),
                                   &buffer_info,
                                   &alloc_info,
                                   &m_vk_buffer,
                                   &m_allocation,
                                   &m_allocation_info))) {
            logger()->error("create buffer");
            return false;
        }
    }

    if (!mapped) {
        if (data) {
            data::ptr map = nullptr;
            if (failed(vmaMapMemory(m_device->alloc(),
                                    m_allocation,
                                    (void**)(&map)))) {
                logger()->error("map buffer memory");
                return false;
            }

            memcpy(map, data, size);

            vmaUnmapMemory(m_device->alloc(),
                           m_allocation);
        }
    } else if (data && m_allocation_info.pMappedData) {
        memcpy(m_allocation_info.pMappedData,
               data,
               size);

        flush();
    }

    m_descriptor.buffer = m_vk_buffer;
    m_descriptor.offset = 0;
    m_descriptor.range = size;

    return true;
}

//-----------------------------------------------------------------------------
bool buffer::create_mapped(device::ptr dev,
                           void const* data,
                           size_t size,
                           VkBufferUsageFlags usage,
                           VmaMemoryUsage memory_usage,
                           VkSharingMode sharing_mode,
                           std::vector<ui32> const& shared_queue_family_indices,
                           i32 alignment) {
    return create(dev,
                  data,
                  size,
                  usage,
                  true,
                  memory_usage,
                  sharing_mode,
                  shared_queue_family_indices,
                  alignment);
}

//-----------------------------------------------------------------------------
void buffer::destroy() {
    if (!m_vk_buffer)
        return;

    vmaDestroyBuffer(m_device->alloc(),
                     m_vk_buffer,
                     m_allocation);

    m_vk_buffer = VK_NULL_HANDLE;
    m_allocation = nullptr;

    m_device = nullptr;
}

//-----------------------------------------------------------------------------
VkDeviceAddress buffer::get_address() const {
    if (m_device->call().vkGetBufferDeviceAddressKHR) {
        VkBufferDeviceAddressInfoKHR addr_info{
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR,
            .buffer = m_vk_buffer};
        return m_device->call().vkGetBufferDeviceAddressKHR(m_device->get(),
                                                            &addr_info);
    } else {
        return 0;
    }
}

//-----------------------------------------------------------------------------
void buffer::flush(VkDeviceSize offset, VkDeviceSize size) {
    vmaFlushAllocation(m_device->alloc(),
                       m_allocation,
                       offset,
                       size);
}

//-----------------------------------------------------------------------------
VkPipelineStageFlags buffer_usage_to_possible_stages(VkBufferUsageFlags usage) {
    VkPipelineStageFlags flags = 0;

    if (usage & (VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT))
        flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    if (usage & (VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
        flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    if (usage & VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT)
        flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
    if (usage
        & (VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
           | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT
           | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT))
        flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
                 | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
                 | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    if (usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
        flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

    return flags;
}

//-----------------------------------------------------------------------------
VkAccessFlags buffer_usage_to_possible_access(VkBufferUsageFlags usage) {
    VkAccessFlags flags = 0;

    if (usage & (VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT))
        flags |= VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
    if (usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
        flags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    if (usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
        flags |= VK_ACCESS_INDEX_READ_BIT;
    if (usage & VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT)
        flags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    if (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
        flags |= VK_ACCESS_UNIFORM_READ_BIT;
    if (usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
        flags |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

    return flags;
}

} // namespace lava
