/**
 * @file         liblava/base/memory.hpp
 * @brief        Vulkan allocator
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

// clang-format off

#include "liblava/base/base.hpp"
#include "liblava/fwd.hpp"
#include "vk_mem_alloc.h"
#include <memory>

// clang-format on

namespace lava {

/**
 * @brief Vulkan allocator
 */
struct allocator {
    /// Shared pointer to a allocator
    using s_ptr = std::shared_ptr<allocator>;

    /// Const pointer to device
    using device_c_ptr = device const*;

    /**
     * @brief Make a new allocator
     * @return s_ptr    Shared pointer to allocator
     */
    static s_ptr make() {
        return std::make_shared<allocator>();
    }

    /**
     * @brief Construct a new allocator
     */
    allocator() = default;

    /**
     * @brief Construct a new allocator
     * @param allocator    VMA allocator
     */
    explicit allocator(VmaAllocator allocator)
    : m_allocator(allocator) {}

    /**
     * @brief Create a new allocator
     * @param device    Vulkan device
     * @param flags     VMA allocator create flags
     * @return Create was successful or failed
     */
    bool create(device_c_ptr device,
                VmaAllocatorCreateFlags flags = 0);

    /**
     * @brief Destroy the allocator
     */
    void destroy();

    /**
     * @brief Check if allocator is valid
     * @return Allocator is valid or not
     */
    bool valid() const {
        return m_allocator != nullptr;
    }

    /**
     * @brief Get the VMA allocator
     * @return VmaAllocator    VMA allocator
     */
    VmaAllocator get() const {
        return m_allocator;
    }

private:
    /// VMA allocator
    VmaAllocator m_allocator = nullptr;
};

/**
 * @brief Create a allocator
 * @param device               Vulkan device
 * @param flags                VMA allocator create flags
 * @return allocator::s_ptr    Allocator
 */
inline allocator::s_ptr create_allocator(allocator::device_c_ptr device,
                                         VmaAllocatorCreateFlags flags = 0) {
    auto result = allocator::make();
    if (!result->create(device, flags))
        return nullptr;

    return result;
}

/**
 * @brief Vulkan memory
 */
struct memory : no_copy_no_move {
    /**
     * @brief Construct a new memory
     */
    memory();

    /**
     * @brief Get memory instance
     * @return memory&    Memory
     */
    static memory& instance() {
        static memory memory;
        return memory;
    }

    /**
     * @brief Get allocation callback
     * @return VkAllocationCallbacks*    Allocation callbacks
     */
    VkAllocationCallbacks* alloc() {
        if (m_use_custom_cpu_callbacks)
            return &m_vk_callbacks;

        return nullptr;
    }

    /**
     * @brief Set the callbacks object
     * @param callbacks    Allocation Callbacks
     */
    void set_callbacks(VkAllocationCallbacks const& callbacks) {
        m_vk_callbacks = callbacks;
    }

    /**
     * @brief Set use custom cpu callbacks
     * @param value    Value state
     */
    void set_use_custom_cpu_callbacks(bool value) {
        m_use_custom_cpu_callbacks = value;
    }

private:
    /// Use custom cpu callbacks
    bool m_use_custom_cpu_callbacks = true;

    /// @see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkAllocationCallbacks.html
    VkAllocationCallbacks m_vk_callbacks = {};
};

/**
 * @brief Find the memory type with properties
 * @param properties             Physical device memory properties
 * @param type_bits              Type bits
 * @param required_properties    Memory property flags
 * @return type                  Result type
 */
index find_memory_type_with_properties(VkPhysicalDeviceMemoryProperties properties,
                                       ui32 type_bits,
                                       VkMemoryPropertyFlags required_properties);

/**
 * @brief Find the memory type
 * @param gpu           Physical device
 * @param properties    Memory properties flags
 * @param type_bits     Type bits
 * @return type         Result type
 */
index find_memory_type(VkPhysicalDevice gpu,
                       VkMemoryPropertyFlags properties,
                       ui32 type_bits);

} // namespace lava
