/**
 * @file         liblava/resource/buffer.hpp
 * @brief        Vulkan buffer
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/base/device.hpp"

namespace lava {

/**
 * @brief Buffer
 */
struct buffer : entity {
    /// Shared pointer to buffer
    using ptr = std::shared_ptr<buffer>;

    /// List of buffers
    using list = std::vector<ptr>;

    /**
     * @brief Make a new buffer
     * @return ptr    Shared pointer to buffer
     */
    static ptr make() {
        return std::make_shared<buffer>();
    }

    /**
     * @brief Destroy the buffer
     */
    ~buffer() {
        destroy();
    }

    /**
     * @brief Create a new buffer
     * @param device          Vulkan device
     * @param data            Buffer data
     * @param size            Data size
     * @param usage           Buffer usage flags
     * @param mapped          Map the buffer
     * @param memory_usage    Memory usage
     * @return Create was successful or failed
     */
    bool create(device_p device,
                void const* data,
                size_t size,
                VkBufferUsageFlags usage,
                bool mapped = false,
                VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_GPU_ONLY);

    /**
     * @brief Create a new mapped buffer
     * @param device          Vulkan device
     * @param data            Buffer data
     * @param size            Data size
     * @param usage           Buffer usage flags
     * @param memory_usage    Memory usage
     * @return Create was successful or failed
     */
    bool create_mapped(device_p device,
                       void const* data,
                       size_t size,
                       VkBufferUsageFlags usage,
                       VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU);

    /**
     * @brief Destroy the buffer
     */
    void destroy();

    /**
     * @brief Get the device
     * @return device_p    Vulkan device
     */
    device_p get_device() {
        return device;
    }

    /**
     * @brief Check if the buffer is valid
     * @return Buffer is valid or not
     */
    bool valid() const {
        return vk_buffer != VK_NULL_HANDLE;
    }

    /**
     * @brief Get the buffer
     * @return VkBuffer    Vulkan buffer
     */
    VkBuffer get() const {
        return vk_buffer;
    }

    /**
     * @brief Get the descriptor information
     * @return VkDescriptorBufferInfo const*    Descriptor buffer information
     */
    VkDescriptorBufferInfo const* get_descriptor_info() const {
        return &descriptor;
    }

    /**
     * @brief Get the address of the buffer
     * @return VkDeviceAddress    Device address
     */
    VkDeviceAddress get_address() const;

    /**
     * @brief Get the size of the buffer
     * @return VkDeviceSize    Device size
     */
    VkDeviceSize get_size() const {
        return allocation_info.size;
    }

    /**
     * @brief Get the mapped data
     * @return void*    Pointer to data
     */
    void* get_mapped_data() const {
        return allocation_info.pMappedData;
    }

    /**
     * @brief Get the device memory of the buffer
     * @return VkDeviceMemory    Device memory
     */
    VkDeviceMemory get_device_memory() const {
        return allocation_info.deviceMemory;
    }

    /**
     * @brief Flush the buffer data
     * @param offset    Offset device size
     * @param size      Data device size
     */
    void flush(VkDeviceSize offset = 0,
               VkDeviceSize size = VK_WHOLE_SIZE);

    /**
     * @brief Get the allocation
     * @return VmaAllocation const&    Allocation
     */
    VmaAllocation const& get_allocation() const {
        return allocation;
    }

    /**
     * @brief Get the allocation information
     * @return VmaAllocationInfo const&    Allocation information
     */
    VmaAllocationInfo const& get_allocation_info() const {
        return allocation_info;
    }

private:
    /// Vulkan device
    device_p device = nullptr;

    /// Vulkan buffer
    VkBuffer vk_buffer = VK_NULL_HANDLE;

    /// Allocation
    VmaAllocation allocation = nullptr;

    /// Allocation information
    VmaAllocationInfo allocation_info = {};

    /// Descriptor buffer information
    VkDescriptorBufferInfo descriptor = {};
};

/**
 * @brief Get possible stages by bufferusage flags
 * @param usage                    Buffer usage flags
 * @return VkPipelineStageFlags    Pipeline stage flags
 */
VkPipelineStageFlags buffer_usage_to_possible_stages(VkBufferUsageFlags usage);

/**
 * @brief Get possible access by buffer usage flags
 * @param usage             Buffer usage flags
 * @return VkAccessFlags    Access flags
 */
VkAccessFlags buffer_usage_to_possible_access(VkBufferUsageFlags usage);

} // namespace lava
