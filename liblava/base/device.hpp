// file      : liblava/base/device.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/base.hpp>
#include <liblava/base/memory.hpp>

namespace lava {

// fwd
struct physical_device;

struct device : no_copy_no_move {

    using ptr = std::shared_ptr<device>;
    using list = std::vector<device::ptr>;

    struct create_param {

        const physical_device* _physical_device = nullptr;

        struct queue_info {

            using list = std::vector<queue_info>;

            VkQueueFlags flags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;

            using priority_list = std::vector<float>;
            priority_list priorities;

            ui32 count() const { return to_ui32(priorities.size()); }

            explicit queue_info(ui32 count = 1) {

                for (auto i = 0u; i < count; ++i)
                    priorities.push_back(1.f);
            }
        };

        queue_info::list queue_info_list;

        names extensions;

        void set_default_queues() {

            extensions.push_back("VK_KHR_swapchain");
            queue_info_list.resize(1);
        }
    };

    ~device();

    bool create(create_param const& param);
    void destroy();

    struct queue {

        using list = std::vector<queue>;
        using ref = queue const&;

        VkQueue vk_queue = nullptr;
        ui32 family_index = 0;
    };

    queue::ref get_graphics_queue(ui32 index = 0) const { return get_graphics_queues().at(index); }
    queue::ref get_compute_queue(ui32 index = 0) const { return get_compute_queues().at(index); }
    queue::ref get_transfer_queue(ui32 index = 0) const { return get_transfer_queues().at(index); }

    queue::list const& get_graphics_queues() const { return graphics_queues; }
    queue::list const& get_compute_queues() const { return compute_queues; }
    queue::list const& get_transfer_queues() const { return transfer_queues; }

    VkDevice get() const { return vk_device; }
    VolkDeviceTable const& call() const { return table; }

    bool wait_for_idle() const { return check(call().vkDeviceWaitIdle(vk_device)); }

    VkDescriptorPool get_descriptor_pool() const { return descriptor_pool; }

    physical_device const* get_physical_device() const { return _physical_device; }
    VkPhysicalDevice get_vk_physical_device() const;

    VkPhysicalDeviceFeatures const& get_features() const;
    VkPhysicalDeviceProperties const& get_properties() const;

    bool is_surface_supported(VkSurfaceKHR surface) const;

    void set_allocator(allocator::ptr value) { _allocator = value; }
    allocator::ptr get_allocator() { return _allocator; }

    VmaAllocator alloc() const { return _allocator != nullptr ? _allocator->get() : nullptr; }

    vk_result vkCreateImageView(const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView) {

        auto result = table.vkCreateImageView(vk_device, pCreateInfo, pAllocator, pView);
        return { check(result), result };
    }

    vk_result vkCreateSampler(const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) {

        auto result = table.vkCreateSampler(vk_device, pCreateInfo, pAllocator, pSampler);
        return { check(result), result };
    }

    vk_result vkCreateShaderModule(const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule) {

        auto result = table.vkCreateShaderModule(vk_device, pCreateInfo, pAllocator, pShaderModule);
        return { check(result), result };
    }

    vk_result vkCreateFence(const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) {

        auto result = table.vkCreateFence(vk_device, pCreateInfo, pAllocator, pFence);
        return { check(result), result };
    }

    vk_result vkCreateSemaphore(const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) {

        auto result = table.vkCreateSemaphore(vk_device, pCreateInfo, pAllocator, pSemaphore);
        return { check(result), result };
    }

    vk_result vkWaitForFences(uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout) {

        auto result = table.vkWaitForFences(vk_device, fenceCount, pFences, waitAll, timeout);
        return { check(result), result };
    }

    vk_result vkAcquireNextImageKHR(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) {

        auto result = table.vkAcquireNextImageKHR(vk_device, swapchain, timeout, semaphore, fence, pImageIndex);
        return { check(result), result };
    }

    vk_result vkResetFences(uint32_t fenceCount, const VkFence* pFences) {

        auto result = table.vkResetFences(vk_device, fenceCount, pFences);
        return { check(result), result };
    }

    vk_result vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {

        auto result = table.vkQueueSubmit(queue, submitCount, pSubmits, fence);
        return { check(result), result };
    }

    vk_result vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) {

        auto result = table.vkQueuePresentKHR(queue, pPresentInfo);
        return { check(result), result };
    }

    vk_result vkCreateSwapchainKHR(const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) {

        auto result = table.vkCreateSwapchainKHR(vk_device, pCreateInfo, pAllocator, pSwapchain);
        return { check(result), result };
    }

    vk_result vkGetSwapchainImagesKHR(VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages) {

        auto result = table.vkGetSwapchainImagesKHR(vk_device, swapchain, pSwapchainImageCount, pSwapchainImages);
        return { check(result), result };
    }

    vk_result vkCreateCommandPool(const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) {

        auto result = table.vkCreateCommandPool(vk_device, pCreateInfo, pAllocator, pCommandPool);
        return { check(result), result };
    }

    vk_result vkAllocateCommandBuffers(const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) {

        auto result = table.vkAllocateCommandBuffers(vk_device, pAllocateInfo, pCommandBuffers);
        return { check(result), result };
    }

    void vkDestroyImageView(VkImageView imageView, const VkAllocationCallbacks* pAllocator) {

        table.vkDestroyImageView(vk_device, imageView, pAllocator);
    }

    void vkDestroyFence(VkFence fence, const VkAllocationCallbacks* pAllocator) {

        table.vkDestroyFence(vk_device, fence, pAllocator);
    }

    void vkDestroySemaphore(VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator) {

        table.vkDestroySemaphore(vk_device, semaphore, pAllocator);
    }

    void vkDestroySwapchainKHR(VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator) {

        table.vkDestroySwapchainKHR(vk_device, swapchain, pAllocator);
    }

    void vkFreeCommandBuffers(VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {

        table.vkFreeCommandBuffers(vk_device, commandPool, commandBufferCount, pCommandBuffers);
    }

    void vkDestroyCommandPool(VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator) {

        table.vkDestroyCommandPool(vk_device, commandPool, pAllocator);
    }

private:
    bool create_descriptor_pool();

    VkDevice vk_device = nullptr;
    physical_device const* _physical_device = nullptr;

    VolkDeviceTable table = {};

    VkDescriptorPool descriptor_pool = nullptr;

    queue::list graphics_queues;
    queue::list compute_queues;
    queue::list transfer_queues;

    allocator::ptr _allocator;
};

struct device_manager {

    device::ptr create();

    device::ptr create(device::create_param const& param);

    device::list const& get_all() const { return list; }

    void wait_idle();

    void clear();

private:
    device::list list;
};

VkShaderModule create_shader_module(device* device, data const& data);

} // lava
