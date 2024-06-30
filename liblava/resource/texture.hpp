/**
 * @file         liblava/resource/texture.hpp
 * @brief        Vulkan texture
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/resource/buffer.hpp"
#include "liblava/resource/image.hpp"

namespace lava {

/**
 * @brief Texture types
 */
enum class texture_type : index {
    none = 0,
    tex_2d,
    array,
    cube_map
};

/**
 * @brief Texture file path with format
 */
struct texture_file {
    /// List of texture files
    using list = std::vector<texture_file>;

    /// File path
    string path;

    /// File format
    VkFormat format = VK_FORMAT_UNDEFINED;
};

/**
 * @brief Texture
 */
struct texture : entity {
    /// Shared pointer to texture
    using s_ptr = std::shared_ptr<texture>;

    /// Map of textures
    using s_map = std::map<id, s_ptr>;

    /// List of textures
    using s_list = std::vector<s_ptr>;

    /**
     * @brief Texture mip level
     */
    struct mip_level {
        /// List of mip levels
        using list = std::vector<mip_level>;

        /// Mip level extent
        uv2 extent{};

        /// Mip level size
        ui32 size = 0;
    };

    /**
     * @brief Texture layer
     */
    struct layer {
        /// List of layers
        using list = std::vector<layer>;

        /// List of mip levels
        mip_level::list levels;
    };

    /**
     * @brief Make a new texture
     * @return s_ptr    Shared pointer to texture
     */
    static s_ptr make() {
        return std::make_shared<texture>();
    }

    /**
     * @brief Destroy the texture
     */
    ~texture() {
        destroy();
    }

    /**
     * @brief Create a new texture
     * @param device    Vulkan device
     * @param size      Texture size
     * @param format    Texture format
     * @param layers    List of layers
     * @param type      Texture type
     * @return Create was successful or failed
     */
    bool create(device::ptr device,
                uv2 size,
                VkFormat format,
                layer::list const& layers = {},
                texture_type type = texture_type::tex_2d);

    /**
     * @brief Destroy the texture
     */
    void destroy();

    /**
     * @brief Upload data to texture
     * @param data         Data to upload
     * @param data_size    Size of data
     * @return Upload was successful or failed
     */
    bool upload(void const* data,
                size_t data_size);

    /**
     * @brief Stage the texture
     * @param cmd_buffer    Command buffer
     * @return Stage was successful or failed
     */
    bool stage(VkCommandBuffer cmd_buffer);

    /**
     * @brief Destroy the upload buffer
     */
    void destroy_upload_buffer();

    /**
     * @brief Get the descriptor information
     * @return VkDescriptorImageInfo const*    Descriptor image information
     */
    VkDescriptorImageInfo const* get_descriptor_info() const {
        return &m_descriptor;
    }

    /**
     * @brief Get the image of the texture
     * @return image::s_ptr    Shared pointer to image
     */
    image::s_ptr get_image() {
        return m_img;
    }

    /**
     * @brief Get the size of the texture
     * @return uv2    Texture size
     */
    uv2 get_size() const {
        return m_img ? m_img->get_size() : uv2();
    }

    /**
     * @brief Get the type of the texture
     * @return texture_type    Texture type
     */
    texture_type get_type() const {
        return m_type;
    }

    /**
     * @brief Get the format of the texture
     * @return VkFormat    Texture format
     */
    VkFormat get_format() const {
        return m_img ? m_img->get_format() : VK_FORMAT_UNDEFINED;
    }

private:
    /// Texture image
    image::s_ptr m_img;

    /// Texture type
    texture_type m_type = texture_type::none;

    /// List of layers
    layer::list m_layers;

    /// Texture sampler
    VkSampler m_sampler = 0;

    /// Descriptor image information
    VkDescriptorImageInfo m_descriptor = {};

    /// Upload buffer
    buffer::s_ptr m_upload_buffer;
};

/**
 * @brief Texture staging
 */
struct staging {
    /// Pointer to staging
    using ptr = staging*;

    /**
     * @brief Add texture for staging
     * @param texture    Texture to stage
     */
    void add(texture::s_ptr texture) {
        m_todo.push_back(texture);
    }

    /**
     * @brief Stage textures
     * @param cmd_buf    Command buffer
     * @param frame      Frame index
     * @return Stage was successful or failed
     */
    bool stage(VkCommandBuffer cmd_buf,
               index frame);

    /**
     * @brief Clear staging
     */
    void clear() {
        m_todo.clear();
        m_staged.clear();
    }

    /**
     * @brief Check if staging is busy
     * @return Staging is busy or not
     */
    bool busy() const {
        return !m_todo.empty() || !m_staged.empty();
    }

private:
    /// List of textures to stage
    texture::s_list m_todo;

    /// Map of textures by frame index
    using frame_stage_map = std::map<index, texture::s_list>;

    /// Map of staged textures
    frame_stage_map m_staged;
};

/// Texture registry
using texture_registry = id_registry<texture, texture_file>;

} // namespace lava
