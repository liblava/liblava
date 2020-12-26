// file      : liblava/base/base.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/math.hpp>
#include <liblava/util.hpp>

// clang-format off

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>

// clang-format on

namespace lava {

    using VkObjectHandle = ui64;
    using VkFormats = std::vector<VkFormat>;

    using VkImages = std::vector<VkImage>;
    using VkImagesRef = VkImages const&;

    using VkImageViews = std::vector<VkImageView>;
    using VkFramebuffers = std::vector<VkFramebuffer>;

    using VkCommandPools = std::vector<VkCommandPool>;
    using VkCommandBuffers = std::vector<VkCommandBuffer>;

    using VkFences = std::vector<VkFence>;
    using VkSemaphores = std::vector<VkSemaphore>;

    using VkPresentModeKHRs = std::vector<VkPresentModeKHR>;

    using VkDescriptorSets = std::vector<VkDescriptorSet>;
    using VkDescriptorSetLayouts = std::vector<VkDescriptorSetLayout>;
    using VkDescriptorSetLayoutBindings = std::vector<VkDescriptorSetLayoutBinding>;

    using VkPushConstantRanges = std::vector<VkPushConstantRange>;

    using VkAttachmentReferences = std::vector<VkAttachmentReference>;

    using VkClearValues = std::vector<VkClearValue>;

    using VkPipelineShaderStageCreateInfos = std::vector<VkPipelineShaderStageCreateInfo>;
    using VkSpecializationMapEntries = std::vector<VkSpecializationMapEntry>;

    using VkVertexInputBindingDescriptions = std::vector<VkVertexInputBindingDescription>;
    using VkVertexInputAttributeDescriptions = std::vector<VkVertexInputAttributeDescription>;

    using VkPipelineColorBlendAttachmentStates = std::vector<VkPipelineColorBlendAttachmentState>;
    using VkDynamicStates = std::vector<VkDynamicState>;

    using VkQueueFamilyPropertiesList = std::vector<VkQueueFamilyProperties>;
    using VkExtensionPropertiesList = std::vector<VkExtensionProperties>;

    using VkLayerPropertiesList = std::vector<VkLayerProperties>;
    using VkExtensionPropertiesList = std::vector<VkExtensionProperties>;

    using VkPhysicalDevices = std::vector<VkPhysicalDevice>;

    bool check(VkResult result);
    inline bool failed(VkResult result) {
        return !check(result);
    }

    string to_string(VkResult result);
    string version_to_string(ui32 version);

    struct vk_result {
        bool state = false;
        VkResult value = VK_NOT_READY;

        operator bool() {
            return state;
        }
    };

    constexpr bool const build_failed = false;
    constexpr bool const build_done = true;

    using VkAttachments = std::vector<VkImageViews>;
    using VkAttachmentsRef = VkAttachments const&;

    struct target_callback {
        using list = std::vector<target_callback*>;

        using created_func = std::function<bool(VkAttachmentsRef, rect)>;
        created_func on_created;

        using destroyed_func = std::function<void()>;
        destroyed_func on_destroyed;
    };

    // limits

    static constexpr ui32 const Vk_Limit_DescriptorSets = 4;
    static constexpr ui32 const Vk_Limit_Bindings = 16;
    static constexpr ui32 const Vk_Limit_Attachments = 8;
    static constexpr ui32 const Vk_Limit_VertexAttribs = 16;
    static constexpr ui32 const Vk_Limit_VertexBuffers = 4;
    static constexpr ui32 const Vk_Limit_PushConstant_Size = 128;
    static constexpr ui32 const Vk_Limit_UBO_Size = 16 * 1024;

} // namespace lava
