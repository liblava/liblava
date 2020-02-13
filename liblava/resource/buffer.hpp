// file      : liblava/resource/buffer.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device.hpp>
#include <liblava/base/memory.hpp>

namespace lava {

struct buffer : id_obj {

    using ptr = std::shared_ptr<buffer>;
    using list = std::vector<ptr>;

    static VkPipelineStageFlags usage_to_possible_stages(VkBufferUsageFlags usage);
    static VkAccessFlags usage_to_possible_access(VkBufferUsageFlags usage);

    ~buffer() { destroy(); }

    bool create(device_ptr device, void const* data, size_t size, VkBufferUsageFlags usage, bool mapped = false,
                                   VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_GPU_ONLY);

    bool create_mapped(device_ptr device, void const* data, size_t size, VkBufferUsageFlags usage,
                                          VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU);

    void destroy();

    device_ptr get_device() { return device; }

    bool valid() const { return vk_buffer != 0; }

    VkBuffer get() const { return vk_buffer; }
    VkDescriptorBufferInfo const* get_descriptor() const { return &descriptor; }
    VkDescriptorBufferInfo const* get_info() const { return get_descriptor(); }

    VkDeviceSize get_size() const { return allocation_info.size; }
    void* get_mapped_data() const { return allocation_info.pMappedData; }
    VkDeviceMemory get_device_memory() const { return allocation_info.deviceMemory; }

    void flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

private:
    device_ptr device = nullptr;

    VkBuffer vk_buffer = 0;
    VmaAllocation allocation = nullptr;

    VmaAllocationInfo allocation_info = {};
    VkDescriptorBufferInfo descriptor = {};
};

inline buffer::ptr make_buffer() { return std::make_shared<buffer>(); }

} // lava
