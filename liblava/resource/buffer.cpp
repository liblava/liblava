/**
 * @file         liblava/resource/buffer.cpp
 * @brief        Vulkan buffer
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/resource/buffer.hpp>
#include <liblava/util/log.hpp>

namespace lava {

//-----------------------------------------------------------------------------
bool buffer::create(device_p d,
                    void const* data,
                    size_t size,
                    VkBufferUsageFlags usage,
                    bool mapped,
                    VmaMemoryUsage memory_usage) {
    device = d;

    VkBufferCreateInfo buffer_info{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
    };

    VmaAllocationCreateFlags const alloc_flags = mapped
                                                     ? VMA_ALLOCATION_CREATE_MAPPED_BIT
                                                     : 0;

    VmaAllocationCreateInfo alloc_info{
        .flags = alloc_flags,
        .usage = memory_usage,
    };

    if (failed(vmaCreateBuffer(device->alloc(),
                               &buffer_info,
                               &alloc_info,
                               &vk_buffer,
                               &allocation,
                               &allocation_info))) {
        log()->error("create buffer");
        return false;
    }

    if (!mapped) {
        if (data) {
            data_ptr map = nullptr;
            if (failed(vmaMapMemory(device->alloc(),
                                    allocation,
                                    (void**) (&map)))) {
                log()->error("map buffer memory");
                return false;
            }

            memcpy(map, data, size);

            vmaUnmapMemory(device->alloc(),
                           allocation);
        }
    } else if (data && allocation_info.pMappedData) {
        memcpy(allocation_info.pMappedData,
               data,
               size);

        flush();
    }

    descriptor.buffer = vk_buffer;
    descriptor.offset = 0;
    descriptor.range = size;

    return true;
}

//-----------------------------------------------------------------------------
bool buffer::create_mapped(device_p d,
                           void const* data,
                           size_t size,
                           VkBufferUsageFlags usage,
                           VmaMemoryUsage memory_usage) {
    return create(d,
                  data,
                  size,
                  usage,
                  true,
                  memory_usage);
}

//-----------------------------------------------------------------------------
void buffer::destroy() {
    if (!vk_buffer)
        return;

    vmaDestroyBuffer(device->alloc(),
                     vk_buffer,
                     allocation);

    vk_buffer = VK_NULL_HANDLE;
    allocation = nullptr;

    device = nullptr;
}

//-----------------------------------------------------------------------------
VkDeviceAddress buffer::get_address() const {
    if (device->call().vkGetBufferDeviceAddressKHR) {
        VkBufferDeviceAddressInfoKHR addr_info{
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR,
            .buffer = vk_buffer
        };
        return device->call().vkGetBufferDeviceAddressKHR(device->get(),
                                                          &addr_info);
    } else {
        return 0;
    }
}

//-----------------------------------------------------------------------------
void buffer::flush(VkDeviceSize offset, VkDeviceSize size) {
    vmaFlushAllocation(device->alloc(),
                       allocation,
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
