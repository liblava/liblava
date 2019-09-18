// file      : liblava/resource/texture.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/resource/buffer.hpp>
#include <liblava/resource/image.hpp>

namespace lava {

BETTER_ENUM(texture_type, type, none = 0, tex_2d, array, cube_map)

struct file_format {

    using list = std::vector<file_format>;

    string path;
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
};

struct texture {

    using ptr = std::shared_ptr<texture>;
    using map = std::map<id, ptr>;
    using list = std::vector<ptr>;

    struct mip_level {

        using list = std::vector<mip_level>;

        uv2 extent;
        ui32 size = 0;
    };

    struct layer {

        using list = std::vector<layer>;

        mip_level::list levels;
    };

    explicit texture();
    ~texture();

    static ptr make() { return std::make_shared<texture>(); }

    bool create(device* device, uv2 size, VkFormat format, layer::list const& layers = {}, texture_type type = texture_type::tex_2d);
    void destroy();

    bool upload(void const* data, size_t data_size);
    bool stage(VkCommandBuffer cmd_buffer);
    void destroy_upload_buffer();

    VkDescriptorImageInfo get_descriptor() const { return descriptor; }
    image::ptr get_image() { return _image; }

    uv2 get_size() const { return _image ? _image->get_size() : uv2(); }
    texture_type get_type() const { return type; }

    id::ref get_id() const { return _id; }

    VkFormat get_format() const { return _image ? _image->get_format() : VK_FORMAT_UNDEFINED; }

private:
    id _id;

    image::ptr _image;

    texture_type type = texture_type::none;
    layer::list layers;

    VkSampler sampler = nullptr;
    VkDescriptorImageInfo descriptor = {};

    buffer::ptr upload_buffer;
};

texture::ptr load_texture(device* device, file_format filename, texture_type type = texture_type::tex_2d);

inline texture::ptr load_texture(device* device, string filename, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, texture_type type = texture_type::tex_2d) {

    return load_texture(device, { filename, format }, type);
}

texture::ptr create_default_texture(device* device, uv2 size = { 512, 512});

struct staging {

    void add(texture::ptr texture) { todo.push_back(texture); }

    bool stage(VkCommandBuffer cmdBuffer, index frame);

private:
    texture::list todo;

    using frame_stage_map = std::map<index, texture::list>;
    frame_stage_map staged;
};

using texture_registry = registry<texture, file_format>;

} // lava
