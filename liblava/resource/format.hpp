// file      : liblava/resource/format.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device.hpp>
#include <liblava/core/data.hpp>

namespace lava {

bool format_depth(VkFormat format);

bool format_stencil(VkFormat format);

bool format_depth_stencil(VkFormat format);

VkImageAspectFlags format_aspect_mask(VkFormat format);

void format_block_dim(VkFormat format, ui32& width, ui32& height);

void format_align_dim(VkFormat format, ui32& width, ui32& height);

void format_num_blocks(VkFormat format, ui32& width, ui32& height);

ui32 format_block_size(VkFormat format);

bool get_supported_depth_format(VkPhysicalDevice physical_device, VkFormat* depth_format);

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

} // lava
