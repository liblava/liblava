// file      : liblava/frame/window.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/data.hpp>
#include <liblava/core/math.hpp>
#include <liblava/frame/input.hpp>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <optional>

// fwd
struct GLFWwindow;

namespace lava {

    constexpr name _default_ = "default";

    constexpr name _window_file_ = "window.json";
    constexpr name _x_ = "x";
    constexpr name _y_ = "y";
    constexpr name _width_ = "width";
    constexpr name _height_ = "height";
    constexpr name _fullscreen_ = "fullscreen";
    constexpr name _floating_ = "floating";
    constexpr name _resizable_ = "resizable";
    constexpr name _decorated_ = "decorated";
    constexpr name _maximized_ = "maximized";
    constexpr name _monitor_ = "monitor";

    struct window : id_obj {
        struct state {
            using optional = std::optional<window::state>;

            explicit state() {}

            i32 x = 0;
            i32 y = 0;
            ui32 width = 0;
            ui32 height = 0;
            bool fullscreen = false;
            bool floating = false;
            bool resizable = true;
            bool decorated = true;
            bool maximized = false;
            index monitor = 0;
        };

        using ptr = std::shared_ptr<window>;
        using event = std::function<void(ptr)>;
        using map = std::map<id, ptr>;
        using ref = window const&;

        window() = default;
        explicit window(name title)
        : title(title) {}

        bool create(state::optional state = {});
        void destroy();

        state get_state() const;

        void set_title(name text);
        name get_title() const {
            return str(title);
        }

        void set_save_name(name save) {
            save_name = save;
        }
        name get_save_name() const {
            return str(save_name);
        }

        void set_position(i32 x, i32 y);
        void get_position(i32& x, i32& y) const;
        void set_size(ui32 width, ui32 height);
        void get_size(ui32& width, ui32& height) const;

        void get_framebuffer_size(ui32& width, ui32& height) const;

        void set_mouse_position(r64 x, r64 y);
        void get_mouse_position(r64& x, r64& y) const;
        mouse_position get_mouse_position() const;

        void hide_mouse_cursor();
        void show_mouse_cursor();

        float get_aspect_ratio() const;

        void show();
        void hide();
        bool visible() const;

        void iconify();
        bool iconified() const;
        void restore();

        void maximize();
        bool maximized() const;

        void focus();
        bool focused() const;

        void set_fullscreen(bool active) {
            if (fullscreen_active != active)
                switch_mode_request_active = true;
        }
        bool fullscreen() const {
            return fullscreen_active;
        }

        bool hovered() const;

        bool resizable() const;
        void set_resizable(bool value);

        bool decorated() const;
        void set_decorated(bool value);

        bool floating() const;
        void set_floating(bool value);

        static window* get_window(GLFWwindow* handle);

        bool close_request() const;
        bool switch_mode_request() const {
            return switch_mode_request_active;
        }

        bool switch_mode(state::optional state = {});

        GLFWwindow* get() const {
            return handle;
        }

        bool resize_request() const {
            return resize_request_active;
        }
        bool handle_resize() {
            if (on_resize)
                if (!on_resize(framebuffer_width, framebuffer_height))
                    return false;

            resize_request_active = false;
            return true;
        }

        void update_state() {
            get_position(pos_x, pos_y);
            get_size(width, height);
        }

        using resize_func = std::function<bool(ui32, ui32)>;
        resize_func on_resize;

        void assign(input* callback) {
            input = callback;
        }

        void set_debug_title(bool value = true) {
            debug_title_active = value;
        }
        bool debug_title() const {
            return debug_title_active;
        }

        void update_title() {
            set_title(str(title));
        }

        VkSurfaceKHR create_surface();

        void set_icon(data_ptr data, uv2 size);

        index get_monitor() const;

        void center();

    private:
        void handle_message();
        void handle_mouse_message();

        GLFWwindow* handle = nullptr;
        lava::input* input = nullptr;

        string title = _lava_;
        string save_name = _default_;

        bool fullscreen_active = false;
        bool debug_title_active = false;

        bool switch_mode_request_active = false;
        bool resize_request_active = false;

        ui32 framebuffer_width = 0;
        ui32 framebuffer_height = 0;

        i32 pos_x = 0;
        i32 pos_y = 0;
        ui32 width = 0;
        ui32 height = 0;
    };

    VkSurfaceKHR create_surface(GLFWwindow* window);

} // namespace lava
