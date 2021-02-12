// file      : liblava/resource/format.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device.hpp>
#include <optional>

namespace lava {

    bool format_depth(VkFormat format);

    bool format_stencil(VkFormat format);

    bool format_depth_stencil(VkFormat format);

    bool format_srgb(VkFormat format);

    VkImageAspectFlags format_aspect_mask(VkFormat format);

    void format_block_dim(VkFormat format, ui32& width, ui32& height);

    void format_align_dim(VkFormat format, ui32& width, ui32& height);

    void format_num_blocks(VkFormat format, ui32& width, ui32& height);

    ui32 format_block_size(VkFormat format);

    std::optional<VkFormat> get_supported_depth_format(VkPhysicalDevice physical_device);

    std::optional<VkFormat> get_supported_format(VkPhysicalDevice physical_device, VkFormats const& possible_formats, VkImageUsageFlags usage);

    VkImageMemoryBarrier image_memory_barrier(VkImage image, VkImageLayout old_layout, VkImageLayout new_layout);

    void set_image_layout(device_ptr device, VkCommandBuffer cmd_buffer, VkImage image, VkImageLayout old_image_layout,
                          VkImageLayout new_image_layout, VkImageSubresourceRange subresource_range,
                          VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                          VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    void set_image_layout(device_ptr device, VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                          VkImageLayout old_image_layout, VkImageLayout new_image_layout,
                          VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                          VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    void insert_image_memory_barrier(device_ptr device, VkCommandBuffer cmd_buffer, VkImage image, VkAccessFlags src_access_mask,
                                     VkAccessFlags dst_access_mask, VkImageLayout old_image_layout,
                                     VkImageLayout new_image_layout, VkPipelineStageFlags src_stage_mask,
                                     VkPipelineStageFlags dst_stage_mask,
                                     VkImageSubresourceRange subresource_range);

    struct surface_format_request {
        VkFormats formats = {
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_FORMAT_B8G8R8_UNORM,
            VK_FORMAT_R8G8B8_UNORM,
            VK_FORMAT_B8G8R8A8_SRGB,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_FORMAT_B8G8R8_SRGB,
            VK_FORMAT_R8G8B8_SRGB,
        };
        VkColorSpaceKHR color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    };
    VkSurfaceFormatKHR get_surface_format(VkPhysicalDevice device, VkSurfaceKHR surface, surface_format_request request = {});

} // namespace lava
