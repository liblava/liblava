/**
 * @file         liblava/block/pipeline_layout.hpp
 * @brief        Pipeline layout
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/block/descriptor.hpp"

namespace lava {

/**
 * @brief Pipeline layout
 */
struct pipeline_layout : entity {
    /// Shared pointer to pipeline layout
    using s_ptr = std::shared_ptr<pipeline_layout>;

    /// List of pipeline layouts
    using s_list = std::vector<s_ptr>;

    /**
     * @brief Make a new pipeline layout
     * @return ptr    Shared pointer to pipeline layout
     */
    static s_ptr make() {
        return std::make_shared<pipeline_layout>();
    }

    /**
     * @see add_descriptor
     */
    void add(descriptor::s_ptr const& descriptor) {
        m_descriptors.push_back(descriptor);
    }

    /**
     * @see add_push_constant_range
     */
    void add(VkPushConstantRange const& range) {
        m_push_constant_ranges.push_back(range);
    }

    /**
     * @brief Add descriptor
     * @param descriptor    Descriptor
     */
    void add_descriptor(descriptor::s_ptr const& descriptor) {
        add(descriptor);
    }

    /**
     * @brief Add push contant range
     * @param range    Push contant range
     */
    void add_push_constant_range(VkPushConstantRange const& range) {
        add(range);
    }

    /**
     * @brief Clear descriptors
     */
    void clear_descriptors() {
        m_descriptors.clear();
    }

    /**
     * @brief Clear push constant ranges
     */
    void clear_ranges() {
        m_push_constant_ranges.clear();
    }

    /**
     * @brief Clear descriptors and push constant ranges
     */
    void clear() {
        clear_descriptors();
        clear_ranges();
    }

    /**
     * @brief Create a new pipeline layout
     * @param device    Vulkan device
     * @return Create was successful or failed
     */
    bool create(device::ptr device);

    /**
     * @brief Destroy the pipeline layout
     */
    void destroy();

    /**
     * @brief Get the Vulkan pipeline layout
     * @return VkPipelineLayout    Pipeline layout
     */
    VkPipelineLayout get() const {
        return m_layout;
    }

    /**
     * @brief Get the device
     * @return device::ptr    Vulkan device
     */
    device::ptr get_device() {
        return m_device;
    }

    /**
     * @brief Get the descriptors
     * @return descriptor::s_list const&    List of descriptors
     */
    descriptor::s_list const& get_descriptors() const {
        return m_descriptors;
    }

    /**
     * @brief Get the push constant ranges
     * @return VkPushConstantRanges const&    List of push constant ranges
     */
    VkPushConstantRanges const& get_push_constant_ranges() const {
        return m_push_constant_ranges;
    }

    /// List of offsets
    using offset_list = std::vector<index>;

    /**
     * @brief Bind descriptor set
     * @param cmd_buf           Command buffer
     * @param descriptor_set    Descriptor set
     * @param first_set         Index to first descriptor set
     * @param offsets           List of offsets
     * @param bind_point        Pipeline bind point
     */
    void bind_descriptor_set(VkCommandBuffer cmd_buf,
                             VkDescriptorSet descriptor_set,
                             index first_set = 0,
                             offset_list offsets = {},
                             VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);

    /**
     * @see bind_descriptor_set
     */
    void bind(VkCommandBuffer cmd_buf,
              VkDescriptorSet descriptor_set,
              index first_set = 0,
              offset_list offsets = {},
              VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS) {
        bind_descriptor_set(cmd_buf,
                            descriptor_set,
                            first_set,
                            offsets,
                            bind_point);
    }

private:
    /// Vulkan device
    device::ptr m_device = nullptr;

    /// Pipeline layout
    VkPipelineLayout m_layout = VK_NULL_HANDLE;

    /// List of descriptors
    descriptor::s_list m_descriptors;

    /// List of push constant ranges
    VkPushConstantRanges m_push_constant_ranges;
};

} // namespace lava
