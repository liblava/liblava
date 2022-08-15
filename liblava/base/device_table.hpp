/**
 * @file         liblava/base/device_table.hpp
 * @brief        Device functions
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/memory.hpp>

namespace lava {

/**
 * @brief Device functions
 */
struct device_table {
    /**
     * @brief Load device table
     */
    void load_table() {
        volkLoadDeviceTable(&table, vk_device);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateImageView
     */
    vk_result vkCreateImageView(const VkImageViewCreateInfo* pCreateInfo,
                                const VkAllocationCallbacks* pAllocator,
                                VkImageView* pView) {
        auto result = table.vkCreateImageView(vk_device,
                                              pCreateInfo,
                                              pAllocator,
                                              pView);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateImageView
     */
    vk_result vkCreateImageView(const VkImageViewCreateInfo* pCreateInfo,
                                VkImageView* pView) {
        return vkCreateImageView(pCreateInfo,
                                 memory::instance().alloc(),
                                 pView);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateSampler
     */
    vk_result vkCreateSampler(const VkSamplerCreateInfo* pCreateInfo,
                              const VkAllocationCallbacks* pAllocator,
                              VkSampler* pSampler) {
        auto result = table.vkCreateSampler(vk_device,
                                            pCreateInfo,
                                            pAllocator,
                                            pSampler);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateSampler
     */
    vk_result vkCreateSampler(const VkSamplerCreateInfo* pCreateInfo,
                              VkSampler* pSampler) {
        return vkCreateSampler(pCreateInfo,
                               memory::instance().alloc(),
                               pSampler);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateShaderModule
     */
    vk_result vkCreateShaderModule(const VkShaderModuleCreateInfo* pCreateInfo,
                                   const VkAllocationCallbacks* pAllocator,
                                   VkShaderModule* pShaderModule) {
        auto result = table.vkCreateShaderModule(vk_device,
                                                 pCreateInfo,
                                                 pAllocator,
                                                 pShaderModule);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateShaderModule
     */
    vk_result vkCreateShaderModule(const VkShaderModuleCreateInfo* pCreateInfo,
                                   VkShaderModule* pShaderModule) {
        return vkCreateShaderModule(pCreateInfo,
                                    memory::instance().alloc(),
                                    pShaderModule);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateFence
     */
    vk_result vkCreateFence(const VkFenceCreateInfo* pCreateInfo,
                            const VkAllocationCallbacks* pAllocator,
                            VkFence* pFence) {
        auto result = table.vkCreateFence(vk_device,
                                          pCreateInfo,
                                          pAllocator,
                                          pFence);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateFence
     */
    vk_result vkCreateFence(const VkFenceCreateInfo* pCreateInfo,
                            VkFence* pFence) {
        return vkCreateFence(pCreateInfo,
                             memory::instance().alloc(),
                             pFence);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateSemaphore
     */
    vk_result vkCreateSemaphore(const VkSemaphoreCreateInfo* pCreateInfo,
                                const VkAllocationCallbacks* pAllocator,
                                VkSemaphore* pSemaphore) {
        auto result = table.vkCreateSemaphore(vk_device,
                                              pCreateInfo,
                                              pAllocator,
                                              pSemaphore);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateSemaphore
     */
    vk_result vkCreateSemaphore(const VkSemaphoreCreateInfo* pCreateInfo,
                                VkSemaphore* pSemaphore) {
        return vkCreateSemaphore(pCreateInfo,
                                 memory::instance().alloc(),
                                 pSemaphore);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkWaitForFences
     */
    vk_result vkWaitForFences(uint32_t fenceCount,
                              const VkFence* pFences,
                              VkBool32 waitAll,
                              uint64_t timeout) {
        auto result = table.vkWaitForFences(vk_device,
                                            fenceCount,
                                            pFences,
                                            waitAll,
                                            timeout);
        if ((result == VK_TIMEOUT) && (timeout != UINT64_MAX))
            return { false, result };

        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkResetFences
     */
    vk_result vkResetFences(uint32_t fenceCount,
                            const VkFence* pFences) {
        auto result = table.vkResetFences(vk_device,
                                          fenceCount,
                                          pFences);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkQueueSubmit
     */
    vk_result vkQueueSubmit(VkQueue queue,
                            uint32_t submitCount,
                            const VkSubmitInfo* pSubmits,
                            VkFence fence) {
        auto result = table.vkQueueSubmit(queue,
                                          submitCount,
                                          pSubmits,
                                          fence);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/vkAcquireNextImageKHR.html
     */
    vk_result vkAcquireNextImageKHR(VkSwapchainKHR swapchain,
                                    uint64_t timeout,
                                    VkSemaphore semaphore,
                                    VkFence fence,
                                    uint32_t* pImageIndex) {
        auto result = table.vkAcquireNextImageKHR(vk_device,
                                                  swapchain,
                                                  timeout,
                                                  semaphore,
                                                  fence,
                                                  pImageIndex);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/vkQueuePresentKHR.html
     */
    vk_result vkQueuePresentKHR(VkQueue queue,
                                const VkPresentInfoKHR* pPresentInfo) {
        auto result = table.vkQueuePresentKHR(queue,
                                              pPresentInfo);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/vkCreateSwapchainKHR.html
     */
    vk_result vkCreateSwapchainKHR(const VkSwapchainCreateInfoKHR* pCreateInfo,
                                   const VkAllocationCallbacks* pAllocator,
                                   VkSwapchainKHR* pSwapchain) {
        auto result = table.vkCreateSwapchainKHR(vk_device,
                                                 pCreateInfo,
                                                 pAllocator,
                                                 pSwapchain);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/vkCreateSwapchainKHR.html
     */
    vk_result vkCreateSwapchainKHR(const VkSwapchainCreateInfoKHR* pCreateInfo,
                                   VkSwapchainKHR* pSwapchain) {
        return vkCreateSwapchainKHR(pCreateInfo,
                                    memory::instance().alloc(),
                                    pSwapchain);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/vkDestroySwapchainKHR.html
     */
    void vkDestroySwapchainKHR(VkSwapchainKHR swapchain,
                               const VkAllocationCallbacks* pAllocator = memory::instance().alloc()) {
        table.vkDestroySwapchainKHR(vk_device,
                                    swapchain,
                                    pAllocator);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/vkGetSwapchainImagesKHR.html
     */
    vk_result vkGetSwapchainImagesKHR(VkSwapchainKHR swapchain,
                                      uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages) {
        auto result = table.vkGetSwapchainImagesKHR(vk_device,
                                                    swapchain,
                                                    pSwapchainImageCount,
                                                    pSwapchainImages);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateCommandPool
     */
    vk_result vkCreateCommandPool(const VkCommandPoolCreateInfo* pCreateInfo,
                                  const VkAllocationCallbacks* pAllocator,
                                  VkCommandPool* pCommandPool) {
        auto result = table.vkCreateCommandPool(vk_device,
                                                pCreateInfo,
                                                pAllocator,
                                                pCommandPool);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateCommandPool
     */
    vk_result vkCreateCommandPool(const VkCommandPoolCreateInfo* pCreateInfo,
                                  VkCommandPool* pCommandPool) {
        return vkCreateCommandPool(pCreateInfo,
                                   memory::instance().alloc(),
                                   pCommandPool);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkCreateCommandPool
     */
    vk_result vkCreateCommandPool(index queue_family,
                                  VkCommandPool* pCommandPool) {
        VkCommandPoolCreateInfo create_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = queue_family,
        };
        return vkCreateCommandPool(&create_info,
                                   pCommandPool);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkAllocateCommandBuffers
     */
    vk_result vkAllocateCommandBuffers(const VkCommandBufferAllocateInfo* pAllocateInfo,
                                       VkCommandBuffer* pCommandBuffers) {
        auto result = table.vkAllocateCommandBuffers(vk_device,
                                                     pAllocateInfo,
                                                     pCommandBuffers);
        return { check(result), result };
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkAllocateCommandBuffers
     */
    vk_result vkAllocateCommandBuffers(VkCommandPool commandPool,
                                       uint32_t commandBufferCount,
                                       VkCommandBuffer* pCommandBuffers,
                                       VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
        VkCommandBufferAllocateInfo alloc_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = commandPool,
            .level = level,
            .commandBufferCount = commandBufferCount,
        };
        return vkAllocateCommandBuffers(&alloc_info,
                                        pCommandBuffers);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkDestroyImageView
     */
    void vkDestroyImageView(VkImageView imageView,
                            const VkAllocationCallbacks* pAllocator = memory::instance().alloc()) {
        table.vkDestroyImageView(vk_device,
                                 imageView,
                                 pAllocator);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkDestroyFence
     */
    void vkDestroyFence(VkFence fence,
                        const VkAllocationCallbacks* pAllocator = memory::instance().alloc()) {
        table.vkDestroyFence(vk_device,
                             fence,
                             pAllocator);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkDestroySemaphore
     */
    void vkDestroySemaphore(VkSemaphore semaphore,
                            const VkAllocationCallbacks* pAllocator = memory::instance().alloc()) {
        table.vkDestroySemaphore(vk_device,
                                 semaphore,
                                 pAllocator);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkFreeCommandBuffers
     */
    void vkFreeCommandBuffers(VkCommandPool commandPool,
                              uint32_t commandBufferCount,
                              const VkCommandBuffer* pCommandBuffers) {
        table.vkFreeCommandBuffers(vk_device,
                                   commandPool,
                                   commandBufferCount,
                                   pCommandBuffers);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkDestroyCommandPool
     */
    void vkDestroyCommandPool(VkCommandPool commandPool,
                              const VkAllocationCallbacks* pAllocator = memory::instance().alloc()) {
        table.vkDestroyCommandPool(vk_device,
                                   commandPool,
                                   pAllocator);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkDestroySampler
     */
    void vkDestroySampler(VkSampler sampler,
                          const VkAllocationCallbacks* pAllocator = memory::instance().alloc()) {
        table.vkDestroySampler(vk_device,
                               sampler,
                               pAllocator);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkUpdateDescriptorSets
     */
    void vkUpdateDescriptorSets(uint32_t descriptorWriteCount,
                                const VkWriteDescriptorSet* pDescriptorWrites,
                                uint32_t descriptorCopyCount = 0,
                                const VkCopyDescriptorSet* pDescriptorCopies = nullptr) {
        table.vkUpdateDescriptorSets(vk_device,
                                     descriptorWriteCount,
                                     pDescriptorWrites,
                                     descriptorCopyCount,
                                     pDescriptorCopies);
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkUpdateDescriptorSets
     */
    template<std::size_t SIZE>
    void vkUpdateDescriptorSets(std::array<VkWriteDescriptorSet, SIZE> const& descriptor_writes) {
        vkUpdateDescriptorSets(to_i32(descriptor_writes.size()),
                               descriptor_writes.data());
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkUpdateDescriptorSets
     */
    template<std::size_t SIZE>
    void vkUpdateDescriptorSets(std::array<VkCopyDescriptorSet, SIZE> const& descriptor_copies) {
        vkUpdateDescriptorSets(0,
                               nullptr,
                               to_i32(descriptor_copies.size()),
                               descriptor_copies.data());
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkUpdateDescriptorSets
     */
    template<std::size_t WRITE_SIZE, std::size_t COPY_SIZE>
    void vkUpdateDescriptorSets(std::array<VkWriteDescriptorSet, WRITE_SIZE> const& descriptor_writes,
                                std::array<VkCopyDescriptorSet, COPY_SIZE> const& descriptor_copies) {
        vkUpdateDescriptorSets(to_i32(descriptor_writes.size()),
                               descriptor_writes.data(),
                               to_i32(descriptor_copies.size()),
                               descriptor_copies.data());
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkUpdateDescriptorSets
     */
    void vkUpdateDescriptorSets(std::initializer_list<VkWriteDescriptorSet> descriptor_writes) {
        vkUpdateDescriptorSets(to_i32(descriptor_writes.size()),
                               descriptor_writes.begin());
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkUpdateDescriptorSets
     */
    void vkUpdateDescriptorSets(std::initializer_list<VkCopyDescriptorSet> descriptor_copies) {
        vkUpdateDescriptorSets(0,
                               nullptr,
                               to_i32(descriptor_copies.size()),
                               descriptor_copies.begin());
    }

    /**
     * @see https://www.khronos.org/registry/vulkan/specs/1.3/html/vkspec.html#vkUpdateDescriptorSets
     */
    void vkUpdateDescriptorSets(std::initializer_list<VkWriteDescriptorSet> descriptor_writes,
                                std::initializer_list<VkCopyDescriptorSet> descriptor_copies) {
        vkUpdateDescriptorSets(to_i32(descriptor_writes.size()),
                               descriptor_writes.begin(),
                               to_i32(descriptor_copies.size()),
                               descriptor_copies.begin());
    }

    /// Vulkan device
    VkDevice vk_device = nullptr;

    /// Volk device table
    VolkDeviceTable table = {};
};

} // namespace lava
