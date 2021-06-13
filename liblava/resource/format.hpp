/**
 * @file         liblava/resource/format.hpp
 * @brief        Vulkan format
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/device.hpp>
#include <optional>

namespace lava {

/// Optional format
using VkFormat_optional = std::optional<VkFormat>;

/**
 * @brief Check if depth format compatible
 * 
 * @param format    Format to check
 * 
 * @return true     Format can depth
 * @return false    No depth supported
 */
bool format_depth(VkFormat format);

/**
 * @brief Check if stencil format compatible
 * 
 * @param format    Format to check
 * 
 * @return true     Format can stencil
 * @return false    No stencil supported
 */
bool format_stencil(VkFormat format);

/**
 * @brief Check if depth or stencil format compatible
 * 
 * @param format    Format to check
 * 
 * @return true     Format can depth or stencil
 * @return false    No depth or stencil supported
 */
bool format_depth_stencil(VkFormat format);

/**
 * @brief Check if sRGB format compatible
 * 
 * @param format    Format to check
 * 
 * @return true     Format can sRGB
 * @return false    No sRGB supported
 */
bool format_srgb(VkFormat format);

/**
 * @brief Get image aspect mask of format
 * 
 * @param format                 Target format
 * 
 * @return VkImageAspectFlags    Image aspect flags
 */
VkImageAspectFlags format_aspect_mask(VkFormat format);

/**
 * @brief Get block dimension of format
 * 
 * @param format    Target format
 * @param width     Block width
 * @param height    Block height
 */
void format_block_dim(VkFormat format, ui32& width, ui32& height);

/**
 * @brief Get align dimension of format
 * 
 * @param format    Target format
 * @param width     Align width
 * @param height    Align height
 */
void format_align_dim(VkFormat format, ui32& width, ui32& height);

/**
 * @brief Get format number of blocks
 * 
 * @param format    Target format
 * @param width     Number blocks width
 * @param height    Number blocks height
 */
void format_num_blocks(VkFormat format, ui32& width, ui32& height);

/**
 * @brief Get format block size
 * 
 * @param format    Target format
 * 
 * @return ui32     Size of block
 */
ui32 format_block_size(VkFormat format);

/**
 * @brief Get the supported depth format
 * 
 * @param physical_device       Physical device
 * 
 * @return VkFormat_optional    Optional format
 */
VkFormat_optional get_supported_depth_format(VkPhysicalDevice physical_device);

/**
 * @brief Get the supported format
 * 
 * @param physical_device       Physical device
 * @param possible_formats      List of possible formats
 * @param usage                 Image usage flags
 * 
 * @return VkFormat_optional    Optional format
 */
VkFormat_optional get_supported_format(VkPhysicalDevice physical_device, VkFormats const& possible_formats, VkImageUsageFlags usage);

/**
 * @brief Get image memory barrier
 * 
 * @param image                    Target image
 * @param old_layout               Old image layout
 * @param new_layout               New image layout
 * 
 * @return VkImageMemoryBarrier    Image memory barrier
 */
VkImageMemoryBarrier image_memory_barrier(VkImage image, VkImageLayout old_layout, VkImageLayout new_layout);

/**
 * @brief Set the image layout
 * 
 * @param device               Vulkan device
 * @param cmd_buffer           Command buffer
 * @param image                Target image
 * @param old_image_layout     Old image layout
 * @param new_image_layout     New image layout
 * @param subresource_range    Image subresource range
 * @param src_stage_mask       Source pipeline stage flags
 * @param dst_stage_mask       Destination pipeline stage flags
 */
void set_image_layout(device_ptr device, VkCommandBuffer cmd_buffer, VkImage image, VkImageLayout old_image_layout,
                      VkImageLayout new_image_layout, VkImageSubresourceRange subresource_range,
                      VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                      VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

/**
 * @brief Set the image layout
 * 
 * @param device              Vulkan device
 * @param cmd_buffer          Command buffer
 * @param image               Target image
 * @param aspect_mask         Image aspect flags
 * @param old_image_layout    Old image layout
 * @param new_image_layout    New image layout
 * @param src_stage_mask      Source pipeline stage flags
 * @param dst_stage_mask      Destination pipeline stage flags
 */
void set_image_layout(device_ptr device, VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask,
                      VkImageLayout old_image_layout, VkImageLayout new_image_layout,
                      VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                      VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

/**
 * @brief Insert image memory barrier
 * 
 * @param device               Vulkan device
 * @param cmd_buffer           Command buffer
 * @param image                Target image
 * @param src_access_mask      Source access mask
 * @param dst_access_mask      Destination access mask
 * @param old_image_layout     Old image layout
 * @param new_image_layout     New image layout
 * @param src_stage_mask       Source pipeline stage flags
 * @param dst_stage_mask       Destination pipeline stage flags
 * @param subresource_range    Image subresource range
 */
void insert_image_memory_barrier(device_ptr device, VkCommandBuffer cmd_buffer, VkImage image, VkAccessFlags src_access_mask,
                                 VkAccessFlags dst_access_mask, VkImageLayout old_image_layout,
                                 VkImageLayout new_image_layout, VkPipelineStageFlags src_stage_mask,
                                 VkPipelineStageFlags dst_stage_mask,
                                 VkImageSubresourceRange subresource_range);

/**
 * @brief Surface format request
 */
struct surface_format_request {
    /// List of formats in request order
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

    /// Color space to request
    VkColorSpaceKHR color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
};

/**
 * @brief Get the surface format
 * 
 * @param device                 Vulkan device
 * @param surface                Vulkan surface
 * @param request                Surface format request
 * 
 * @return VkSurfaceFormatKHR    Chosen surface format
 */
VkSurfaceFormatKHR get_surface_format(VkPhysicalDevice device, VkSurfaceKHR surface, surface_format_request request = {});

} // namespace lava
