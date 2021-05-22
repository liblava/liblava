// file      : liblava/app/imgui.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/block/pipeline.hpp>
#include <liblava/file.hpp>
#include <liblava/frame/input.hpp>
#include <liblava/resource/texture.hpp>

// fwd
struct GLFWwindow;
struct GLFWcursor;
struct ImDrawData;
struct ImGuiStyle;

namespace lava {

    constexpr const r32 default_imgui_font_size = 18.f;
    constexpr name _imgui_file_ = "imgui.ini";

    struct imgui : input_callback {
        explicit imgui() = default;
        explicit imgui(GLFWwindow* window) {
            setup(window);
        }
        ~imgui() {
            destroy();
        }

        struct icon_font {
            data font_data;

            ui16 range_begin = 0;
            ui16 range_end = 0;

            r32 size = default_imgui_font_size;
        };

        struct font {
            using ref = font const&;

            string file;
            r32 size = 21.f;

            string icon_file;
            r32 icon_size = 21.f;

            ui16 icon_range_begin = 0;
            ui16 icon_range_end = 0;
        };

        struct config {
            data font_data;
            r32 font_size = default_imgui_font_size;

            std::shared_ptr<ImGuiStyle> style;

            icon_font icon;

            fs::path ini_file_dir;
        };

        void setup(GLFWwindow* window, config config);
        void setup(GLFWwindow* window) {
            setup(window, config());
        }

        bool create(graphics_pipeline::ptr pipeline, index max_frames);
        bool create(device_ptr device, index max_frames) {
            return create(make_graphics_pipeline(device), max_frames);
        }
        bool create(device_ptr device, index max_frames, VkRenderPass pass) {
            if (!create(device, max_frames))
                return false;

            return pipeline->create(pass);
        }

        bool upload_fonts(texture::ptr texture);

        void destroy();

        bool ready() const {
            return initialized;
        }
        graphics_pipeline::ptr get_pipeline() {
            return pipeline;
        }

        using draw_func = std::function<void()>;
        draw_func on_draw;

        bool capture_mouse() const;
        bool capture_keyboard() const;

        void set_active(bool value = true) {
            active = value;
        }
        bool activated() const {
            return active;
        }

        void toggle() {
            active = !active;
        }

        void set_ini_file(fs::path dir);
        fs::path get_ini_file() const {
            return fs::path(ini_file);
        }

        void convert_style_to_srgb();

    private:
        void handle_key_event(i32 key, i32 scancode, i32 action, i32 mods);
        void handle_mouse_button_event(i32 button, i32 action, i32 mods);
        void handle_scroll_event(r64 x_offset, r64 y_offset);

        void prepare_draw_lists(ImDrawData* draw_data);
        void render_draw_lists(VkCommandBuffer cmd_buf);
        void invalidate_device_objects();

        void update_mouse_pos_and_buttons();
        void update_mouse_cursor();

        void new_frame();
        void render(VkCommandBuffer cmd_buf);

        device_ptr device = nullptr;
        bool initialized = false;

        graphics_pipeline::ptr pipeline;
        pipeline_layout::ptr layout;

        size_t buffer_memory_alignment = 256;
        index frame = 0;
        index max_frames = 4;

        buffer::list vertex_buffers;
        buffer::list index_buffers;

        lava::descriptor::ptr descriptor;
        lava::descriptor::pool::ptr descriptor_pool;
        VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

        GLFWwindow* window = nullptr;

        bool mouse_just_pressed[5] = { false, false, false, false, false };
        r64 current_time = 0.0;

        std::vector<GLFWcursor*> mouse_cursors;

        string ini_file;

        bool active = true;
    };

    void setup_imgui_font(imgui::config& config, imgui::font::ref font);

} // namespace lava
