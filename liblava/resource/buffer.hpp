// file      : liblava/resource/buffer.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device.hpp>
#include <liblava/base/memory.hpp>

namespace lava {

struct buffer {

    using ptr = std::shared_ptr<buffer>;
    using list = std::vector<ptr>;

    static VkPipelineStageFlags usage_to_possible_stages(VkBufferUsageFlags usage);
    static VkAccessFlags usage_to_possible_access(VkBufferUsageFlags usage);

    explicit buffer();
    ~buffer();

    static ptr make() { return std::make_shared<buffer>(); }

    bool create(device* device, void const* data, size_t size, VkBufferUsageFlags usage, bool mapped = false, 
                                VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY);
    void destroy();

    id::ref get_id() const { return _id; }
    device* get_device() { return dev; }

    bool is_valid() const { return vk_buffer != nullptr; }

    VkBuffer get() const { return vk_buffer; }
    VkDescriptorBufferInfo const& get_descriptor() const { return descriptor; }

    size_t get_size() const { return allocation_info.size; }
    void* get_mapped_data() const { return allocation_info.pMappedData; }
    VkDeviceMemory get_device_memory() const { return allocation_info.deviceMemory; }

    void flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

private:
    id _id;
    device* dev = nullptr;

    VkBuffer vk_buffer = nullptr;
    VmaAllocation allocation = nullptr;

    VmaAllocationInfo allocation_info = {};
    VkDescriptorBufferInfo descriptor = {};
};

} // lava
