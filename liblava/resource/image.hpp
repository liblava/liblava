// file      : liblava/resource/image.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device.hpp>
#include <liblava/base/memory.hpp>
#include <liblava/core/id.hpp>
#include <liblava/core/math.hpp>

namespace lava {

struct image : id_obj {

    using ptr = std::shared_ptr<image>;
    using map = std::map<id, ptr>;
    using list = std::vector<ptr>;

    explicit image(VkFormat format, VkImage vk_image = 0);

    bool create(device_ptr device, uv2 size, VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_GPU_ONLY, bool mip_levels_generation = false);
    void destroy(bool view_only = false);
    void destroy_view() { destroy(true); }

    device_ptr get_device() { return device; }

    VkFormat get_format() const { return info.format; }
    uv2 get_size() const { return { info.extent.width, info.extent.height }; }
    ui32 get_depth() const { return info.extent.depth; }

    VkImage get() const { return vk_image; }
    VkImageView get_view() const { return view; }

    VkImageCreateInfo const& get_info() const { return info; }
    VkImageViewCreateInfo const& get_view_info() const { return view_info; }
    VkImageSubresourceRange const& get_subresource_range() const { return subresource_range; }

    void set_flags(VkImageCreateFlagBits flags) { info.flags = flags; }
    void set_tiling(VkImageTiling tiling) { info.tiling = tiling; }
    void set_usage(VkImageUsageFlags usage) { info.usage = usage; }
    void set_layout(VkImageLayout initial) { info.initialLayout = initial; }

    void set_aspect_mask(VkImageAspectFlags aspectMask) { subresource_range.aspectMask = aspectMask; }

    void set_level_count(ui32 levels) { subresource_range.levelCount = levels; info.mipLevels = levels; }
    void set_layer_count(ui32 layers) { subresource_range.layerCount = layers; info.arrayLayers = layers; }

    void set_component(VkComponentMapping mapping = {}) { view_info.components = mapping; }
    void set_view_type(VkImageViewType type) { view_info.viewType = type; }

private:
    device_ptr device = nullptr;

    VkImage vk_image = 0;
    VkImageCreateInfo info;

    VmaAllocation allocation = nullptr;

    VkImageView view = 0;

    VkImageViewCreateInfo view_info;
    VkImageSubresourceRange subresource_range;
};

image::ptr make_image(VkFormat format, VkImage vk_image = 0);
image::ptr make_image(VkFormat format, device_ptr device, uv2 size, VkImage vk_image = 0);

} // lava
