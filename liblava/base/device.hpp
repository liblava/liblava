/**
 * @file         liblava/base/device.hpp
 * @brief        Vulkan device
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/device_table.hpp>
#include <liblava/base/queue.hpp>
#include <liblava/core/data.hpp>
#include <liblava/core/id.hpp>
#include <liblava/fwd.hpp>

namespace lava {

/// Pointer to device
using device_p = device*;

/// Const pointer to device
using device_cp = device const*;

/// Const pointer to a physical device
using physical_device_cp = physical_device const*;

/**
 * @brief Vulkan device
 */
struct device : device_table, entity {
    /// Shared pointer to a device
    using ptr = std::shared_ptr<device>;

    /// List of devices
    using list = std::vector<device::ptr>;

    /**
     * @brief Device create parameters
     */
    struct create_param {
        /// Reference to device create parameters
        using ref = create_param const&;

        /// Physical device
        physical_device_cp physical_device = nullptr;

        /// VMA flags
        VmaAllocatorCreateFlags vma_flags = 0;

        /// List of extensions to enable
        names extensions;

        /// List of physical device features to enable
        VkPhysicalDeviceFeatures features{};

        /// Must be true if .next points to a VkPhysicalDevice2 instance
        bool has_features_2 = false;

        /// Create parameter next pointer (pNext)
        void const* next = nullptr;

        /// List of queue famiy infos
        queue_family_info::list queue_family_infos;

        /**
         * @brief Add swapchain extension
         */
        void add_swapchain_extension() {
            extensions.push_back("VK_KHR_swapchain");
        }

        /**
         * @brief Add portability subset extension
         */
        void add_portability_subset_extension() {
            extensions.push_back("VK_KHR_portability_subset");
        }

        /**
         * @brief Set the default queues
         */
        void set_default_queues() {
            lava::set_default_queues(queue_family_infos);
        }

        /**
         * @brief Set the all queues
         */
        void set_all_queues();

        /**
         * @brief Add queue
         * @param flags       Queue flags
         * @param priority    Priority for queue
         * @return Add was successful or failed
         */
        bool add_queue(VkQueueFlags flags,
                       r32 priority = 1.f) {
            return add_queues(flags, 1, priority);
        }

        /**
         * @brief Add queues
         * @param flags       Queue flags
         * @param count       Number of queues
         * @param priority    Priority for queues
         * @return Add was successful or failed
         */
        bool add_queues(VkQueueFlags flags,
                        ui32 count,
                        r32 priority = 1.f);

        /**
         * @brief Add all dedicated queues
         * @param priority    Priority for queues
         * @return Add was successful or failed
         */
        bool add_dedicated_queues(r32 priority = 1.f);

        /**
         * @brief Verify queues
         * @return verify_queues_result    Verification result
         */
        verify_queues_result verify_queues() const;
    };

    /**
     * @brief Destroy the device
     */
    ~device() {
        destroy();
    }

    /**
     * @brief Create a new device
     * @param param     Create parameters
     * @return Create was successful or failed
     */
    bool create(create_param::ref param);

    /**
     * @brief Destroy the device
     */
    void destroy();

    /**
     * @brief Get a graphics queue by index
     * @param index          Index of queue
     * @return queue::ref    Graphics queue
     */
    queue::ref get_graphics_queue(index index = 0) const {
        return get_graphics_queues().at(index);
    }

    /**
     * @see get_graphics_queue
     */
    queue::ref graphics_queue(index index = 0) const {
        return get_graphics_queue(index);
    }

    /**
     * @brief Get a compute queue by index
     * @param index          Index of queue
     * @return queue::ref    Compute queue
     */
    queue::ref get_compute_queue(index index = 0) const {
        return get_compute_queues().at(index);
    }

    /**
     * @see get_compute_queue
     */
    queue::ref compute_queue(index index = 0) const {
        return get_compute_queue(index);
    }

    /**
     * @brief Get a transfer queue by index
     * @param index          Index of queue
     * @return queue::ref    Transfer queue
     */
    queue::ref get_transfer_queue(index index = 0) const {
        return get_transfer_queues().at(index);
    }

    /**
     * @see get_transfer_queue
     */
    queue::ref transfer_queue(index index = 0) const {
        return get_transfer_queue(index);
    }

    /**
     * @brief Get the list of graphics queues
     * @return queue::list const&    Graphics queues
     */
    queue::list const& get_graphics_queues() const {
        return graphics_queue_list;
    }

    /**
     * @see get_graphics_queues
     */
    queue::list const& graphics_queues() const {
        return get_graphics_queues();
    }

    /**
     * @brief Get the list of compute queues
     * @return queue::list const&    Compute queues
     */
    queue::list const& get_compute_queues() const {
        return compute_queue_list;
    }

    /**
     * @see get_compute_queues
     */
    queue::list const& compute_queues() const {
        return get_compute_queues();
    }

    /**
     * @brief Get the list of transfer queues
     * @return queue::list const&    Transfer queues
     */
    queue::list const& get_transfer_queues() const {
        return transfer_queue_list;
    }

    /**
     * @see get_transfer_queues
     */
    queue::list const& transfer_queues() const {
        return get_transfer_queues();
    }

    /**
     * @brief Get all queues
     * @return queue::list const&    List of all queues
     */
    queue::list const& get_queues() const {
        return queue_list;
    }

    /**
     * @see get_queues
     */
    queue::list const& queues() const {
        return get_queues();
    }

    /**
     * @brief Get the Vulkan device
     * @return VkDevice    Vulkan device
     */
    VkDevice get() const {
        return vk_device;
    }

    /**
     * @brief Get the Volk device table
     * @return VolkDeviceTable const&    Volk device table
     */
    VolkDeviceTable const& call() const {
        return table;
    }

    /**
     * @brief Wait for idle
     * @return Wait was successful or failed
     */
    bool wait_for_idle() const {
        return check(call().vkDeviceWaitIdle(vk_device));
    }

    /**
     * @brief Get the physical device
     * @return physical_device_cp    Physical device
     */
    physical_device_cp get_physical_device() const {
        return physical_device;
    }

    /**
     * @brief Get the Vulkan physical device
     * @return VkPhysicalDevice    Vulkan physical device
     */
    VkPhysicalDevice get_vk_physical_device() const;

    /**
     * @brief Get the physical device features
     * @return VkPhysicalDeviceFeatures const&    Features
     */
    VkPhysicalDeviceFeatures const& get_features() const;

    /**
     * @brief Get the physical device properties
     * @return VkPhysicalDeviceProperties const&    Properties
     */
    VkPhysicalDeviceProperties const& get_properties() const;

    /**
     * @brief Check if surface is supported by this device
     * @param surface    Surface to check
     * @return Surface is supported or not
     */
    bool surface_supported(VkSurfaceKHR surface) const;

    /**
     * @brief Set the allocator for this device
     * @param value    Allocator
     */
    void set_allocator(allocator::ptr value) {
        mem_allocator = value;
    }

    /**
     * @brief Get the allocator of this device
     * @return allocator::ptr    Allocator
     */
    allocator::ptr get_allocator() {
        return mem_allocator;
    }

    /**
     * @brief Get the VMA allocator
     * @return VmaAllocator    VMA allocator
     */
    VmaAllocator alloc() const {
        return mem_allocator != nullptr
                   ? mem_allocator->get()
                   : nullptr;
    }

private:
    /// Physical device
    physical_device_cp physical_device = nullptr;

    /// List of qraphics queues
    queue::list graphics_queue_list;

    /// List of compute queues
    queue::list compute_queue_list;

    /// List of transfer queues
    queue::list transfer_queue_list;

    /// List of all queues
    queue::list queue_list;

    /// Device features
    VkPhysicalDeviceFeatures features{};

    /// Device allocator
    allocator::ptr mem_allocator;
};

/**
 * @brief Create a shader module
 * @param device             Vulkan device
 * @param data               Shader data
 * @return VkShaderModule    Shader module
 */
VkShaderModule create_shader_module(device_p device,
                                    cdata::ref data);

/// One time command function
using one_time_command_func = std::function<void(VkCommandBuffer)>;

/**
 * @brief Submit one time command function with pool
 * @param device      Vulkan device
 * @param pool        Command pool (VK_NULL_HANDLE: managed)
 * @param queue       Target queue
 * @param callback    Function to be called
 * @return Submit was successful or failed
 */
bool one_time_submit_pool(device_p device,
                          VkCommandPool pool,
                          queue::ref queue,
                          one_time_command_func callback);

/**
 * @brief Submit one time command function
 * @param device      Vulkan device
 * @param queue       Target queue
 * @param callback    Function to be called
 * @return Submit was successful or failed
 */
inline bool one_time_submit(device_p device,
                            queue::ref queue,
                            one_time_command_func callback) {
    return one_time_submit_pool(device,
                                VK_NULL_HANDLE,
                                queue,
                                callback);
}

} // namespace lava
