/**
 * @file liblava/resource/image.hpp
 * @brief Vulkan image
 * @authors Lava Block OÜ and contributors
 * @copyright Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/device.hpp>

namespace lava {

/**
 * @brief Image
 */
struct image : id_obj {
    /// Shared pointer to image
    using ptr = std::shared_ptr<image>;

    /// Map of images
    using map = std::map<id, ptr>;

    /// List of images
    using list = std::vector<ptr>;

    /**
     * @brief Construct a new image
     * 
     * @param format Image format
     * @param vk_image Vulkan image
     */
    explicit image(VkFormat format, VkImage vk_image = 0);

    /**
     * @brief Create a new image
     * 
     * @param device Vulkan device
     * @param size Image size
     * @param memory_usage Memory usage
     * @param mip_levels_generation Enable mip levels generation
     * @return true Create was successful
     * @return false Create failed
     */
    bool create(device_ptr device, uv2 size, VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_GPU_ONLY, bool mip_levels_generation = false);

    /**
     * @brief Destroy the image
     * 
     * @param view_only Destroy only the image view
     */
    void destroy(bool view_only = false);

    /**
     * @brief Destroy the image view
     */
    void destroy_view() {
        destroy(true);
    }

    /**
     * @brief Get the device
     * 
     * @return device_ptr Vulkan device
     */
    device_ptr get_device() {
        return device;
    }

    /**
     * @brief Get the format of the image
     * 
     * @return VkFormat Image format
     */
    VkFormat get_format() const {
        return info.format;
    }

    /**
     * @brief Get the size of the image
     * 
     * @return uv2 Image size
     */
    uv2 get_size() const {
        return { info.extent.width, info.extent.height };
    }

    /**
     * @brief Get the depth of the image
     * 
     * @return ui32 Image depth
     */
    ui32 get_depth() const {
        return info.extent.depth;
    }

    /**
     * @brief Get the image
     * 
     * @return VkImage Vulkan image
     */
    VkImage get() const {
        return vk_image;
    }

    /**
     * @brief Get the image view
     * 
     * @return VkImageView Vulkan image view
     */
    VkImageView get_view() const {
        return view;
    }

    /**
     * @brief Get the image create information
     * 
     * @return VkImageCreateInfo const& Image create information
     */
    VkImageCreateInfo const& get_info() const {
        return info;
    }

    /**
     * @brief Get the image view create information
     * 
     * @return VkImageViewCreateInfo const& Image view create information
     */
    VkImageViewCreateInfo const& get_view_info() const {
        return view_info;
    }

    /**
     * @brief Get the subresource range of the image
     * 
     * @return VkImageSubresourceRange const& Image subresource range
     */
    VkImageSubresourceRange const& get_subresource_range() const {
        return subresource_range;
    }

    /**
     * @brief Set the image create flags
     * 
     * @param flags Image create flag bits
     */
    void set_flags(VkImageCreateFlagBits flags) {
        info.flags = flags;
    }

    /**
     * @brief Set the image tiling
     * 
     * @param tiling Image tiling
     */
    void set_tiling(VkImageTiling tiling) {
        info.tiling = tiling;
    }

    /**
     * @brief Set the image usage
     * 
     * @param usage Image usage flags
     */
    void set_usage(VkImageUsageFlags usage) {
        info.usage = usage;
    }

    /**
     * @brief Set the initial layout of the image
     * 
     * @param initial Initial image layout
     */
    void set_layout(VkImageLayout initial) {
        info.initialLayout = initial;
    }

    /**
     * @brief Set the aspect mask of the image
     * 
     * @param aspectMask Image aspect flags
     */
    void set_aspect_mask(VkImageAspectFlags aspectMask) {
        subresource_range.aspectMask = aspectMask;
    }

    /**
     * @brief Set the level count of the image
     * 
     * @param levels Number of levels
     */
    void set_level_count(ui32 levels) {
        subresource_range.levelCount = levels;
        info.mipLevels = levels;
    }

    /**
     * @brief Set the layer count of the image
     * 
     * @param layers Number of layers
     */
    void set_layer_count(ui32 layers) {
        subresource_range.layerCount = layers;
        info.arrayLayers = layers;
    }

    /**
     * @brief Set the component mapping of the image
     * 
     * @param mapping Component mapping
     */
    void set_component(VkComponentMapping mapping = {}) {
        view_info.components = mapping;
    }

    /**
     * @brief Set the view type of the image
     * 
     * @param type Image view type
     */
    void set_view_type(VkImageViewType type) {
        view_info.viewType = type;
    }

private:
    /// Vulkan device
    device_ptr device = nullptr;

    /// Vulkan image
    VkImage vk_image = VK_NULL_HANDLE;

    /// Image create information
    VkImageCreateInfo info;

    /// Allocation
    VmaAllocation allocation = nullptr;

    /// Vulkan image view
    VkImageView view = VK_NULL_HANDLE;

    /// Image view create information
    VkImageViewCreateInfo view_info;

    /// Image subresource range
    VkImageSubresourceRange subresource_range;
};

/**
 * @brief Create a new image
 * 
 * @param format Image format
 * @param vk_image Vulkan image
 * @return image::ptr Shared pointer to image
 */
image::ptr make_image(VkFormat format, VkImage vk_image = 0);

/**
 * @brief Make a new image
 * 
 * @param format Image format
 * @param device Vulkan device
 * @param size Image size
 * @param vk_image Vulkan image
 * @return image::ptr Shared pointer to image
 */
image::ptr make_image(VkFormat format, device_ptr device, uv2 size, VkImage vk_image = 0);

} // namespace lava
