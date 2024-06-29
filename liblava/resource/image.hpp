/**
 * @file         liblava/resource/image.hpp
 * @brief        Vulkan image
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/base/device.hpp"

namespace lava {

/**
 * @brief Image data
 */
struct image_data {
    /// Shared pointer to image data
    using s_ptr = std::shared_ptr<image_data>;

    /// Pointer to image data
    data::ptr data = nullptr;

    /// Dimensions
    uv2 dimensions = uv2(0, 0);

    /// Number of channels
    ui32 channels = 0;

    /**
     * @brief Check if image data is ready
     * @return Image data is ready or not
     */
    bool ready() const {
        return data != nullptr;
    }

    /**
     * @brief Get image data size
     * @return size_t    Image data size
     */
    size_t size() const {
        return channels * dimensions.x * dimensions.y;
    }

    /**
     * @brief Destroy the image data
     */
    ~image_data();
};

/**
 * @brief Image
 */
struct image : entity {
    /// Shared pointer to image
    using s_ptr = std::shared_ptr<image>;

    /// Map of images
    using s_map = std::map<id, s_ptr>;

    /// List of images
    using s_list = std::vector<s_ptr>;

    /**
     * @brief Make a new image
     * @param format      Image format
     * @param vk_image    Vulkan image
     * @return s_ptr      Shared pointer to image
     */
    static s_ptr make(VkFormat format,
                      VkImage vk_image = 0) {
        return std::make_shared<image>(format, vk_image);
    }

    /**
     * @brief Construct a new image
     * @param format      Image format
     * @param vk_image    Vulkan image
     */
    explicit image(VkFormat format,
                   VkImage vk_image = 0);

    /**
     * @brief Create a new image
     * @param device              Vulkan device
     * @param size                Image size
     * @param memory_usage        Memory usage
     * @param allocation_flags    Allocation flags
     * @return Create was successful or failed
     */
    bool create(device::ptr device,
                uv2 size,
                VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_GPU_ONLY,
                VmaAllocationCreateFlags allocation_flags = 0);

    /**
     * @brief Destroy the image
     * @param view_only    Destroy only the image view
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
     * @return device::ptr    Vulkan device
     */
    device::ptr get_device() {
        return m_device;
    }

    /**
     * @brief Get the format of the image
     * @return VkFormat    Image format
     */
    VkFormat get_format() const {
        return m_info.format;
    }

    /**
     * @brief Get the size of the image
     * @return uv2    Image size
     */
    uv2 get_size() const {
        return {m_info.extent.width, m_info.extent.height};
    }

    /**
     * @brief Get the depth of the image
     * @return ui32    Image depth
     */
    ui32 get_depth() const {
        return m_info.extent.depth;
    }

    /**
     * @brief Get the image
     * @return VkImage    Vulkan image
     */
    VkImage get() const {
        return m_vk_image;
    }

    /**
     * @brief Get the image view
     * @return VkImageView    Vulkan image view
     */
    VkImageView get_view() const {
        return m_view;
    }

    /**
     * @brief Get the image create information
     * @return VkImageCreateInfo const&    Image create information
     */
    VkImageCreateInfo const& get_info() const {
        return m_info;
    }

    /**
     * @brief Get the image view create information
     * @return VkImageViewCreateInfo const&    Image view create information
     */
    VkImageViewCreateInfo const& get_view_info() const {
        return m_view_info;
    }

    /**
     * @brief Get the subresource range of the image
     * @return VkImageSubresourceRange const&    Image subresource range
     */
    VkImageSubresourceRange const& get_subresource_range() const {
        return m_subresource_range;
    }

    /**
     * @brief Set the image create flags
     * @param flags    Image create flag bits
     */
    void set_flags(VkImageCreateFlagBits flags) {
        m_info.flags = flags;
    }

    /**
     * @brief Set the image tiling
     * @param tiling    Image tiling
     */
    void set_tiling(VkImageTiling tiling) {
        m_info.tiling = tiling;
    }

    /**
     * @brief Set the image usage
     * @param usage    Image usage flags
     */
    void set_usage(VkImageUsageFlags usage) {
        m_info.usage = usage;
    }

    /**
     * @brief Set the initial layout of the image
     * @param initial    Initial image layout
     */
    void set_layout(VkImageLayout initial) {
        m_info.initialLayout = initial;
    }

    /**
     * @brief Set the aspect mask of the image
     * @param aspectMask    Image aspect flags
     */
    void set_aspect_mask(VkImageAspectFlags aspectMask) {
        m_subresource_range.aspectMask = aspectMask;
    }

    /**
     * @brief Set the level count of the image
     * @param levels    Number of levels
     */
    void set_level_count(ui32 levels) {
        m_subresource_range.levelCount = levels;
        m_info.mipLevels = levels;
    }

    /**
     * @brief Set the layer count of the image
     * @param layers    Number of layers
     */
    void set_layer_count(ui32 layers) {
        m_subresource_range.layerCount = layers;
        m_info.arrayLayers = layers;
    }

    /**
     * @brief Set the component mapping of the image
     * @param mapping    Component mapping
     */
    void set_component(VkComponentMapping mapping = {}) {
        m_view_info.components = mapping;
    }

    /**
     * @brief Set the view type of the image
     * @param type    Image view type
     */
    void set_view_type(VkImageViewType type) {
        m_view_info.viewType = type;
    }

    /**
     * @brief Get the allocation of the image
     * @return VmaAllocation const&    Image allocation
     */
    VmaAllocation const& get_allocation() const {
        return m_allocation;
    }

private:
    /// Vulkan device
    device::ptr m_device = nullptr;

    /// Vulkan image
    VkImage m_vk_image = VK_NULL_HANDLE;

    /// Image create information
    VkImageCreateInfo m_info;

    /// Allocation
    VmaAllocation m_allocation = nullptr;

    /// Vulkan image view
    VkImageView m_view = VK_NULL_HANDLE;

    /// Image view create information
    VkImageViewCreateInfo m_view_info;

    /// Image subresource range
    VkImageSubresourceRange m_subresource_range;
};

/**
 * @brief Create a new image
 * @param device           Vulkan device
 * @param format           Image format
 * @param size             Image size
 * @param vk_image         Vulkan image
 * @return image::s_ptr    Shared pointer to image
 */
image::s_ptr create_image(device::ptr device,
                          VkFormat format,
                          uv2 size,
                          VkImage vk_image = 0);

/**
 * @brief Grab an image (with blit/copy)
 * @param source           Source image
 * @return image::s_ptr    Grabbed image
 */
image::s_ptr grab_image(image::s_ptr source);

} // namespace lava
