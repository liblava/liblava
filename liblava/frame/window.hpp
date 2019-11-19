// file      : liblava/frame/window.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/frame/input.hpp>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

// fwd
struct GLFWwindow;

namespace lava {

struct window : id_obj {

    struct state {

        i32 x = 0;
        i32 y = 0;
        ui32 width = 0;
        ui32 height = 0;
        bool fullscreen = false;
        bool floating = false;
        bool resizable = true;
        bool decorated = true;
        bool maximized = false;
    };

    using ptr = std::shared_ptr<window>;
    using event = std::function<void(ptr)>;
    using map = std::map<id, ptr>;

    window() = default;
    explicit window(name title) : title(title) {}

    bool create(name save_name = "0", state const* state = nullptr);
    void destroy();

    state get_state() const;

    void set_title(name text);
    name get_title() const { return str(title); }

    name get_save_name() const { return str(save_name); }

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

        if (windowed == active)
            switch_mode_request = true;

        windowed = !active;
    }
    bool fullscreen() const { return !windowed; }

    bool hovered() const;

    bool resizable() const;
    void set_resizable(bool value);

    bool decorated() const;
    void set_decorated(bool value);

    bool floating() const;
    void set_floating(bool value);

    static window* get_window(GLFWwindow* handle);

    bool has_close_request() const;
    bool has_switch_mode_request() const { return switch_mode_request; }

    bool switch_mode();

    GLFWwindow* get() const { return handle; }

    bool has_resize_request() const { return resize_request; }
    bool handle_resize() {

        if (on_resize)
            if (!on_resize(width, height))
                return false;

        resize_request = false;
        return true;
    }

    using resize_func = std::function<bool(ui32, ui32)>;
    resize_func on_resize;

    void assign(input* callback) { input = callback; }

    void set_debug_title(bool value = true) { debug_title = value; }
    bool has_debug_title() const { return debug_title; }

    void update_title() { set_title(str(title)); }

    VkSurfaceKHR create_surface();

private:
    void handle_message();

    GLFWwindow* handle = nullptr;
    lava::input* input = nullptr;

    string title = _lava_;
    string save_name;

    bool windowed = true;
    bool switch_mode_request = false;
    bool debug_title = false;

    bool resize_request = false;
    ui32 width = 0;
    ui32 height = 0;
};

VkSurfaceKHR create_surface(GLFWwindow* window);

} // lava
