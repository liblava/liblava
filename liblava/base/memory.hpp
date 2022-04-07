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
     * @brief Construct a new allocator
     */
    allocator() = default;

    /**
     * @brief Construct a new allocator
     *
     * @param allocator    Vma allocator
     */
    explicit allocator(VmaAllocator allocator)
    : vma_allocator(allocator) {}

    /**
     * @brief Create a new allocator
     *
     * @param device    Vulkan device
     * @param flags     Vma allocator create flags
     *
     * @return true     Create was successfal
     * @return false    Create failed
     */
    bool create(device_cptr device, VmaAllocatorCreateFlags flags = 0);

    /**
     * @brief Destroy the allocator
     */
    void destroy();

    /**
     * @brief Check if allocator is valid
     *
     * @return true     Allocator is valid
     * @return false    Allocator is invalid
     */
    bool valid() const {
        return vma_allocator != nullptr;
    }

    /**
     * @brief Get the Vma allocator
     *
     * @return VmaAllocator    Vma allocator
     */
    VmaAllocator get() const {
        return vma_allocator;
    }

private:
    /// Vma allocator
    VmaAllocator vma_allocator = nullptr;
};

/**
 * @brief Make a new allocator
 *
 * @return allocator::ptr    Allacator
 */
inline allocator::ptr make_allocator() {
    return std::make_shared<allocator>();
}

/**
 * @brief Create a allocator
 *
 * @param device             Vulkan device
 * @param flags              Vma allocator create flags
 *
 * @return allocator::ptr    Allocator
 */
inline allocator::ptr create_allocator(device_cptr device, VmaAllocatorCreateFlags flags = 0) {
    auto result = make_allocator();
    if (!result->create(device, flags))
        return nullptr;

    return result;
}

/**
 * @brief Vulkan memory
 */
struct memory : no_copy_no_move {
    /**
     * @brief Get memory singleton
     *
     * @return memory&    Memory
     */
    static memory& get() {
        static memory memory;
        return memory;
    }

    /**
     * @brief Get allocation callback
     *
     * @return VkAllocationCallbacks*    Allocation callbacks
     */
    static VkAllocationCallbacks* alloc() {
        if (get().use_custom_cpu_callbacks)
            return &get().vk_callbacks;

        return nullptr;
    }

    /**
     * @brief Find the type with properties
     *
     * @param properties             Physical device memory properties
     * @param type_bits              Type bits
     * @param required_properties    Memory property flags
     *
     * @return type                  Result type
     */
    static type find_type_with_properties(VkPhysicalDeviceMemoryProperties properties, ui32 type_bits,
                                          VkMemoryPropertyFlags required_properties);

    /**
     * @brief Find the type
     *
     * @param gpu           Physical device
     * @param properties    Memory properties flags
     * @param type_bits     Type bits
     *
     * @return type         Result type
     */
    static type find_type(VkPhysicalDevice gpu, VkMemoryPropertyFlags properties, ui32 type_bits);

    /**
     * @brief Set the callbacks object
     *
     * @param callbacks    Allocation Callbacks
     */
    void set_callbacks(VkAllocationCallbacks const& callbacks) {
        vk_callbacks = callbacks;
    }

    /**
     * @brief Set use custom cpu callbacks
     *
     * @param value    Value state
     */
    void set_use_custom_cpu_callbacks(bool value) {
        use_custom_cpu_callbacks = value;
    }

private:
    /**
     * @brief Construct a new memory
     */
    memory();

    /// Use custom cpu callbacks
    bool use_custom_cpu_callbacks = true;

    /// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkAllocationCallbacks.html
    VkAllocationCallbacks vk_callbacks = {};
};

} // namespace lava
