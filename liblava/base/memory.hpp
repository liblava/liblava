/**
 * @file         liblava/base/memory.hpp
 * @brief        Vulkan allocator
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

// clang-format off

#include <liblava/base/base.hpp>
#include <liblava/fwd.hpp>
#include <vk_mem_alloc.h>
#include <memory>

// clang-format on

namespace lava {

/// Const pointer to device
using device_cptr = device const*;

/**
 * @brief Vulkan allocator
 */
struct allocator {
    /// Shared pointer to a allocator
    using ptr = std::shared_ptr<allocator>;

    /**
     * @brief Make a new allocator
     * @return ptr    Shared pointer to allocator
     */
    static ptr make() {
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
    : vma_allocator(allocator) {}

    /**
     * @brief Create a new allocator
     * @param device    Vulkan device
     * @param flags     VMA allocator create flags
     * @return Create was successful or failed
     */
    bool create(device_cptr device,
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
        return vma_allocator != nullptr;
    }

    /**
     * @brief Get the VMA allocator
     * @return VmaAllocator    VMA allocator
     */
    VmaAllocator get() const {
        return vma_allocator;
    }

private:
    /// VMA allocator
    VmaAllocator vma_allocator = nullptr;
};

/**
 * @brief Create a allocator
 * @param device             Vulkan device
 * @param flags              VMA allocator create flags
 * @return allocator::ptr    Allocator
 */
inline allocator::ptr create_allocator(device_cptr device,
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
        if (use_custom_cpu_callbacks)
            return &vk_callbacks;

        return nullptr;
    }

    /**
     * @brief Set the callbacks object
     * @param callbacks    Allocation Callbacks
     */
    void set_callbacks(VkAllocationCallbacks const& callbacks) {
        vk_callbacks = callbacks;
    }

    /**
     * @brief Set use custom cpu callbacks
     * @param value    Value state
     */
    void set_use_custom_cpu_callbacks(bool value) {
        use_custom_cpu_callbacks = value;
    }

private:
    /// Use custom cpu callbacks
    bool use_custom_cpu_callbacks = true;

    /// @see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkAllocationCallbacks.html
    VkAllocationCallbacks vk_callbacks = {};
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
