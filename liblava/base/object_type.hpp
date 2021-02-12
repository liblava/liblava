// file      : liblava/base/object_type.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/base.hpp>

// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkObjectType.html

namespace lava {

    template<typename T>
    struct object_type {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_UNKNOWN;
    };

    template<>
    struct object_type<VkInstance> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_INSTANCE;
    };

    template<>
    struct object_type<VkPhysicalDevice> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_PHYSICAL_DEVICE;
    };

    template<>
    struct object_type<VkDevice> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_DEVICE;
    };

    template<>
    struct object_type<VkQueue> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_QUEUE;
    };

    template<>
    struct object_type<VkSemaphore> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_SEMAPHORE;
    };

    template<>
    struct object_type<VkCommandBuffer> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_COMMAND_BUFFER;
    };

    template<>
    struct object_type<VkFence> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_FENCE;
    };

    template<>
    struct object_type<VkDeviceMemory> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_DEVICE_MEMORY;
    };

    template<>
    struct object_type<VkBuffer> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_BUFFER;
    };

    template<>
    struct object_type<VkImage> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_IMAGE;
    };

    template<>
    struct object_type<VkEvent> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_EVENT;
    };

    template<>
    struct object_type<VkQueryPool> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_QUERY_POOL;
    };

    template<>
    struct object_type<VkBufferView> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_BUFFER_VIEW;
    };

    template<>
    struct object_type<VkImageView> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_IMAGE_VIEW;
    };

    template<>
    struct object_type<VkShaderModule> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_SHADER_MODULE;
    };

    template<>
    struct object_type<VkPipelineCache> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_PIPELINE_CACHE;
    };

    template<>
    struct object_type<VkPipelineLayout> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
    };

    template<>
    struct object_type<VkRenderPass> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_RENDER_PASS;
    };

    template<>
    struct object_type<VkPipeline> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_PIPELINE;
    };

    template<>
    struct object_type<VkDescriptorSetLayout> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
    };

    template<>
    struct object_type<VkSampler> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_SAMPLER;
    };

    template<>
    struct object_type<VkDescriptorPool> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_DESCRIPTOR_POOL;
    };

    template<>
    struct object_type<VkDescriptorSet> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_DESCRIPTOR_SET;
    };

    template<>
    struct object_type<VkFramebuffer> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_FRAMEBUFFER;
    };

    template<>
    struct object_type<VkCommandPool> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_COMMAND_POOL;
    };

    template<>
    struct object_type<VkSamplerYcbcrConversion> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION;
    };

    template<>
    struct object_type<VkDescriptorUpdateTemplate> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE;
    };

    template<>
    struct object_type<VkSurfaceKHR> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_SURFACE_KHR;
    };

    template<>
    struct object_type<VkSwapchainKHR> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_SWAPCHAIN_KHR;
    };

    template<>
    struct object_type<VkDisplayKHR> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_DISPLAY_KHR;
    };

    template<>
    struct object_type<VkDisplayModeKHR> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_DISPLAY_MODE_KHR;
    };

    template<>
    struct object_type<VkDebugReportCallbackEXT> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT;
    };

    template<>
    struct object_type<VkDebugUtilsMessengerEXT> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT;
    };

    template<>
    struct object_type<VkAccelerationStructureKHR> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR;
    };

    template<>
    struct object_type<VkValidationCacheEXT> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_VALIDATION_CACHE_EXT;
    };

    template<>
    struct object_type<VkAccelerationStructureNV> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV;
    };

    template<>
    struct object_type<VkPerformanceConfigurationINTEL> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_PERFORMANCE_CONFIGURATION_INTEL;
    };

    template<>
    struct object_type<VkDeferredOperationKHR> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR;
    };

    template<>
    struct object_type<VkIndirectCommandsLayoutNV> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV;
    };

    template<>
    struct object_type<VkPrivateDataSlotEXT> {
        static constexpr VkObjectType value = VK_OBJECT_TYPE_PRIVATE_DATA_SLOT_EXT;
    };

} // namespace lava
