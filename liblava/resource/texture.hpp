/**
 * @file         liblava/resource/texture.hpp
 * @brief        Vulkan texture
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/resource/buffer.hpp>
#include <liblava/resource/image.hpp>

namespace lava {

/**
 * @brief Texture types
 */
enum class texture_type : type {
    none = 0,
    tex_2d,
    array,
    cube_map
};

/**
 * @brief File format
 */
struct file_format {
    /// List of file formats
    using list = std::vector<file_format>;

    /// File path
    string path;

    /// File format
    VkFormat format = VK_FORMAT_UNDEFINED;
};

/**
 * @brief Texture
 */
struct texture : id_obj {
    /// Shared pointer to texture
    using ptr = std::shared_ptr<texture>;

    /// Map of textures
    using map = std::map<id, ptr>;

    /// List of textures
    using list = std::vector<ptr>;

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
     * @brief Destroy the texture
     */
    ~texture() {
        destroy();
    }

    /**
     * @brief Create a new texture
     * 
     * @param device    Vulkan device
     * @param size      Texture size
     * @param format    Texture format
     * @param layers    List of layers
     * @param type      Texture type
     * 
     * @return true     Create was successful
     * @return false    Create failed
     */
    bool create(device_ptr device, uv2 size, VkFormat format,
                layer::list const& layers = {}, texture_type type = texture_type::tex_2d);

    /**
     * @brief Destroy the texture
     */
    void destroy();

    /**
     * @brief Upload data to texture
     * 
     * @param data         Data to upload
     * @param data_size    Size of data
     * 
     * @return true        Upload was successful
     * @return false       Upload failed
     */
    bool upload(void const* data, size_t data_size);

    /**
     * @brief Stage the texture
     * 
     * @param cmd_buffer    Command buffer
     * 
     * @return true         Stage was successful
     * @return false        Stage failed
     */
    bool stage(VkCommandBuffer cmd_buffer);

    /**
     * @brief Destroy the upload buffer
     */
    void destroy_upload_buffer();

    /**
     * @brief Get the descriptor information
     * 
     * @return VkDescriptorImageInfo const*    Descriptor image information
     */
    VkDescriptorImageInfo const* get_descriptor_info() const {
        return &descriptor;
    }

    /**
     * @brief Get the image of the texture
     * 
     * @return image::ptr    Shared pointer to image
     */
    image::ptr get_image() {
        return img;
    }

    /**
     * @brief Get the size of the texture
     * 
     * @return uv2    Texture size
     */
    uv2 get_size() const {
        return img ? img->get_size() : uv2();
    }

    /**
     * @brief Get the type of the texture
     * 
     * @return texture_type    Texture type
     */
    texture_type get_type() const {
        return type;
    }

    /**
     * @brief Get the format of the texture
     * 
     * @return VkFormat    Texture format
     */
    VkFormat get_format() const {
        return img ? img->get_format() : VK_FORMAT_UNDEFINED;
    }

private:
    /// Texture image
    image::ptr img;

    /// Texture type
    texture_type type = texture_type::none;

    /// List of layers
    layer::list layers;

    /// Texture sampler
    VkSampler sampler = 0;

    /// Descriptor image information
    VkDescriptorImageInfo descriptor = {};

    /// Upload buffer
    buffer::ptr upload_buffer;
};

/**
 * @brief Make a new texture
 * 
 * @return texture::ptr    Shared pointer to texture
 */
inline texture::ptr make_texture() {
    return std::make_shared<texture>();
}

/**
 * @brief Texture staging
 */
struct staging {
    /**
     * @brief Add texture for staging
     * 
     * @param texture    Texture to stage
     */
    void add(texture::ptr texture) {
        todo.push_back(texture);
    }

    /**
     * @brief Stage textures
     * 
     * @param cmd_buf    Command buffer
     * @param frame      Frame index
     * 
     * @return true      Stage was successful
     * @return false     Stage failed
     */
    bool stage(VkCommandBuffer cmd_buf, index frame);

    /**
     * @brief Clear staging
     */
    void clear() {
        todo.clear();
        staged.clear();
    }

    /**
     * @brief Check if staging is busy
     * 
     * @return true     Staging is busy
     * @return false    Staging is not busy
     */
    bool busy() const {
        return !todo.empty() || !staged.empty();
    }

private:
    /// List of textures to stage
    texture::list todo;

    /// Map of textures by frame index
    using frame_stage_map = std::map<index, texture::list>;

    /// Map of staged textures
    frame_stage_map staged;
};

/// Texture registry
using texture_registry = id_registry<texture, file_format>;

} // namespace lava
