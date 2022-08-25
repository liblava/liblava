/**
 * @file         liblava/base/memory.cpp
 * @brief        Vulkan allocator
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/base/device.hpp>
#include <liblava/base/instance.hpp>
#include <liblava/base/memory.hpp>

#ifdef _WIN32
    #pragma warning(push, 4)
    #pragma warning(disable : 4127) // conditional expression is constant
    #pragma warning(disable : 4100) // unreferenced formal parameter
    #pragma warning(disable : 4189) // local variable is initialized but not referenced
    #pragma warning(disable : 4324) // structure was padded due to alignment specifier
    #pragma warning(disable : 4244) // conversion, possible loss of data
#else
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    #pragma GCC diagnostic ignored "-Wunused-variable"
    #pragma GCC diagnostic ignored "-Wtype-limits"
#endif

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#ifdef _WIN32
    #pragma warning(pop)
#else
    #pragma GCC diagnostic pop
#endif

#define LAVA_CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA (void*) (intptr_t) 20180208

namespace lava {

/**
 * @brief Custom CPU allocation callback
 * @param user_data           User data
 * @param size                Size to allocate
 * @param alignment           Memory alignment
 * @param allocation_scope    System allocation scope
 * @return void*              Allocated data
 */
void* VKAPI_PTR custom_cpu_allocation(void* user_data,
                                      size_t size,
                                      size_t alignment,
                                      VkSystemAllocationScope allocation_scope) {
    LAVA_ASSERT(user_data == LAVA_CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA);
    return alloc_data(size, alignment);
}

/**
 * @brief Custom CPU reallocation callback
 * @param user_data           User data
 * @param original            Pointer to original data
 * @param size                Size to reallocate
 * @param alignment           Memory alignment
 * @param allocation_scope    System allocation scope
 * @return void*              Reallocated data
 */
void* VKAPI_PTR custom_cpu_reallocation(void* user_data,
                                        void* original,
                                        size_t size,
                                        size_t alignment,
                                        VkSystemAllocationScope allocation_scope) {
    LAVA_ASSERT(user_data == LAVA_CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA);
    return realloc_data(original, size, alignment);
}

/**
 * @brief Custom CPU free callback
 * @param user_data    User data
 * @param memory       Memory to free
 */
void VKAPI_PTR custom_cpu_free(void* user_data,
                               void* memory) {
    LAVA_ASSERT(user_data == LAVA_CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA);
    free_data(memory);
}

//-----------------------------------------------------------------------------
memory::memory() {
    if (!use_custom_cpu_callbacks)
        return;

    vk_callbacks.pUserData = LAVA_CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA;

    vk_callbacks.pfnAllocation = reinterpret_cast<PFN_vkAllocationFunction>(
        &custom_cpu_allocation);
    vk_callbacks.pfnReallocation = reinterpret_cast<PFN_vkReallocationFunction>(
        &custom_cpu_reallocation);
    vk_callbacks.pfnFree = reinterpret_cast<PFN_vkFreeFunction>(&custom_cpu_free);
}

//-----------------------------------------------------------------------------
bool allocator::create(device_cptr device, VmaAllocatorCreateFlags flags) {
    VmaVulkanFunctions const vulkan_function {
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        .vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory = device->call().vkAllocateMemory,
        .vkFreeMemory = device->call().vkFreeMemory,
        .vkMapMemory = device->call().vkMapMemory,
        .vkUnmapMemory = device->call().vkUnmapMemory,
        .vkFlushMappedMemoryRanges = device->call().vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges = device->call().vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory = device->call().vkBindBufferMemory,
        .vkBindImageMemory = device->call().vkBindImageMemory,
        .vkGetBufferMemoryRequirements = device->call().vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements = device->call().vkGetImageMemoryRequirements,
        .vkCreateBuffer = device->call().vkCreateBuffer,
        .vkDestroyBuffer = device->call().vkDestroyBuffer,
        .vkCreateImage = device->call().vkCreateImage,
        .vkDestroyImage = device->call().vkDestroyImage,
        .vkCmdCopyBuffer = device->call().vkCmdCopyBuffer,
#if VMA_DEDICATED_ALLOCATION
        .vkGetBufferMemoryRequirements2KHR =
            device->call().vkGetBufferMemoryRequirements2KHR,
        .vkGetImageMemoryRequirements2KHR =
            device->call().vkGetImageMemoryRequirements2KHR,
#endif
#if VMA_BIND_MEMORY2
        .vkBindBufferMemory2KHR = device->call().vkBindBufferMemory2KHR,
        .vkBindImageMemory2KHR = device->call().vkBindImageMemory2KHR,
#endif
#if VMA_MEMORY_BUDGET
        .vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR
#endif
    };

    VmaAllocatorCreateInfo const allocator_info{
        .flags = flags,
        .physicalDevice = device->get_vk_physical_device(),
        .device = device->get(),
        .pAllocationCallbacks = memory::instance().alloc(),
        .pVulkanFunctions = &vulkan_function,
        .instance = instance::singleton().get(),
    };

    return check(vmaCreateAllocator(&allocator_info, &vma_allocator));
}

//-----------------------------------------------------------------------------
void allocator::destroy() {
    if (!vma_allocator)
        return;

    vmaDestroyAllocator(vma_allocator);
    vma_allocator = nullptr;
}

//-----------------------------------------------------------------------------
type find_memory_type_with_properties(VkPhysicalDeviceMemoryProperties properties,
                                      ui32 type_bits,
                                      VkMemoryPropertyFlags required_properties) {
    auto bits = type_bits;
    auto len = std::min(properties.memoryTypeCount, 32u);

    for (auto i = 0u; i < len; ++i) {
        if ((bits & 1) == 1)
            if ((properties.memoryTypes[i].propertyFlags & required_properties) == required_properties)
                return (int) i;

        bits >>= 1;
    }

    return no_type;
}

//-----------------------------------------------------------------------------
type find_memory_type(VkPhysicalDevice gpu,
                      VkMemoryPropertyFlags properties,
                      ui32 type_bits) {
    VkPhysicalDeviceMemoryProperties prop{};
    vkGetPhysicalDeviceMemoryProperties(gpu, &prop);

    for (auto i = 0u; i < prop.memoryTypeCount; ++i)
        if (((prop.memoryTypes[i].propertyFlags & properties) == properties)
            && (type_bits & (1 << i)))
            return i;

    return no_type;
}

} // namespace lava
