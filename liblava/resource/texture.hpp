// file      : liblava/resource/texture.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/resource/buffer.hpp>
#include <liblava/resource/image.hpp>

namespace lava {

    enum class texture_type : type {
        none = 0,
        tex_2d,
        array,
        cube_map
    };

    struct file_format {
        using list = std::vector<file_format>;

        string path;
        VkFormat format = VK_FORMAT_UNDEFINED;
    };

    struct texture : id_obj {
        using ptr = std::shared_ptr<texture>;
        using map = std::map<id, ptr>;
        using list = std::vector<ptr>;

        struct mip_level {
            using list = std::vector<mip_level>;

            uv2 extent{};
            ui32 size = 0;
        };

        struct layer {
            using list = std::vector<layer>;

            mip_level::list levels;
        };

        ~texture() {
            destroy();
        }

        bool create(device_ptr device, uv2 size, VkFormat format,
                    layer::list const& layers = {}, texture_type type = texture_type::tex_2d);
        void destroy();

        bool upload(void const* data, size_t data_size);
        bool stage(VkCommandBuffer cmd_buffer);
        void destroy_upload_buffer();

        VkDescriptorImageInfo const* get_descriptor() const {
            return &descriptor;
        }
        VkDescriptorImageInfo const* get_info() const {
            return get_descriptor();
        }

        image::ptr get_image() {
            return img;
        }

        uv2 get_size() const {
            return img ? img->get_size() : uv2();
        }
        texture_type get_type() const {
            return type;
        }

        VkFormat get_format() const {
            return img ? img->get_format() : VK_FORMAT_UNDEFINED;
        }

    private:
        image::ptr img;

        texture_type type = texture_type::none;
        layer::list layers;

        VkSampler sampler = 0;
        VkDescriptorImageInfo descriptor = {};

        buffer::ptr upload_buffer;
    };

    inline texture::ptr make_texture() {
        return std::make_shared<texture>();
    }

    struct staging {
        void add(texture::ptr texture) {
            todo.push_back(texture);
        }

        bool stage(VkCommandBuffer cmd_buf, index frame);

        void clear() {
            todo.clear();
            staged.clear();
        }

        bool busy() const {
            return !todo.empty() || !staged.empty();
        }

    private:
        texture::list todo;

        using frame_stage_map = std::map<index, texture::list>;
        frame_stage_map staged;
    };

    using texture_registry = id_registry<texture, file_format>;

} // namespace lava
