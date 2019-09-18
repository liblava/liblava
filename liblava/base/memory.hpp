// file      : liblava/base/memory.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/base.hpp>

#include <vk_mem_alloc.h>

namespace lava {

struct allocator {

    explicit allocator(VkPhysicalDevice physical_device, VkDevice device);
    ~allocator();

    using ptr = std::shared_ptr<allocator>;

    static ptr make(VkPhysicalDevice physical_device, VkDevice device) {

        return std::make_shared<allocator>(physical_device, device);
    }

    bool valid() const { return vma_allocator != nullptr; }

    VmaAllocator get() const { return vma_allocator; }

private:
    VmaAllocator vma_allocator = nullptr;
};

struct memory : no_copy_no_move {

    static memory& get() {

        static memory memory;
        return memory;
    }

    static VkAllocationCallbacks* alloc() {

        if (get().use_custom_cpu_callbacks)
            return &get().vk_callbacks;

        return nullptr;
    }

    static type find_type_with_properties(VkPhysicalDeviceMemoryProperties properties, ui32 type_bits,
                                            VkMemoryPropertyFlags required_properties);

    static type find_type(VkPhysicalDevice gpu, VkMemoryPropertyFlags properties, ui32 type_bits);

    void set_callbacks(VkAllocationCallbacks const& callbacks) { vk_callbacks = callbacks; }
    void set_use_custom_cpu_callbacks(bool value) { use_custom_cpu_callbacks = value; }

private:
    memory();

    bool use_custom_cpu_callbacks = true;
    VkAllocationCallbacks vk_callbacks = {};
};

} // lava
