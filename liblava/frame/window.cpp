// file      : liblava/frame/window.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/frame/window.hpp>

#include <liblava/base/instance.hpp>
#include <liblava/base/device.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace lava {

bool window::create(state::optional state) {

    auto primary = glfwGetPrimaryMonitor();
    auto mode = glfwGetVideoMode(primary);

    string default_title = title;
    if (debug_title)
        default_title = fmt::format("%s [%s]", str(title), str(save_name));

    if (state) {

        windowed = !state->fullscreen;

        if (state->fullscreen) {

            handle = glfwCreateWindow(mode->width, mode->height, str(default_title), primary, nullptr);
            if (!handle) {

                log()->error("create fullscreen window (state)");
                return false;
            }
        }
        else {

            handle = glfwCreateWindow(state->width, state->height, str(default_title), nullptr, nullptr);
            if (!handle) {

                log()->error("create window (state)");
                return false;
            }

            glfwSetWindowPos(handle, state->x, state->y);
        }

        pos_x = state->x;
        pos_y = state->y;
        width = state->width;
        height = state->height;

        set_floating(state->floating);
        set_resizable(state->resizable);
        set_decorated(state->decorated);

        if (state->maximized)
            maximize();
    } 
    else {

        pos_x = mode->width / 4;
        pos_y = mode->height / 4;
        width = mode->width / 2;
        height = mode->height / 2;

        if (!windowed) {

            handle = glfwCreateWindow(mode->width, mode->height, str(default_title), primary, nullptr);
            if (!handle) {

                log()->error("create fullscreen window");
                return false;
            }
        }
        else {

            handle = glfwCreateWindow(width, height, str(default_title), nullptr, nullptr);
            if (!handle) {

                log()->error("create window");
                return false;
            }

            glfwSetWindowPos(handle, pos_x, pos_y);
        }
    }

    switch_mode_request = false;
    handle_message();

    return true;
}

void window::destroy() {

    input = nullptr;

    glfwDestroyWindow(handle);
    handle = nullptr;
}

window::state window::get_state() const {

    window::state state;

    state.fullscreen = fullscreen();
    if (state.fullscreen) {

        state.x = pos_x;
        state.y = pos_y;
        state.width = width;
        state.height = height;
    }
    else {

        get_position(state.x, state.y);
        get_size(state.width, state.height);
    }
    
    state.floating = floating();
    state.resizable = resizable();
    state.decorated = decorated();
    state.maximized = maximized();

    return state;
}

void window::set_title(name text) {

    title = text;

    if (!handle)
        return;

    if (debug_title)
        glfwSetWindowTitle(handle, str(fmt::format("%s [%s]", str(title), str(save_name))));
    else
        glfwSetWindowTitle(handle, str(title));
}

bool window::switch_mode(state::optional state) {

    destroy();

    return create(state);
}

void window::handle_message() {

    glfwSetWindowUserPointer(handle, this);

    glfwSetFramebufferSizeCallback(handle, [](GLFWwindow* handle, i32 width, i32 height) {

        auto window = get_window(handle);
        if (!window)
            return;

        window->framebuffer_width = to_ui32(width);
        window->framebuffer_height = to_ui32(height);
        window->resize_request = true;
    });

    glfwSetKeyCallback(handle, [](GLFWwindow* handle, i32 key, i32 scancode, i32 action, i32 mods) {

        auto window = get_window(handle);
        if (!window)
            return;

        window->input->key.add({ window->get_id(), lava::key(key), lava::action(action), lava::mod(mods), scancode });
    });

    glfwSetScrollCallback(handle, [](GLFWwindow* handle, r64 x_offset, r64 y_offset) {

        auto window = get_window(handle);
        if (!window)
            return;

        if (window->input)
            window->input->scroll.add({ window->get_id(), x_offset, y_offset });
    });

    glfwSetMouseButtonCallback(handle, [](GLFWwindow* handle, i32 button, i32 action, i32 mods) {

        auto window = get_window(handle);
        if (!window)
            return;

        if (window->input)
            window->input->mouse_button.add({ window->get_id(), mouse_button(button), lava::action(action), lava::mod(mods) });
    });

    glfwSetCursorPosCallback(handle, [](GLFWwindow* handle, r64 x_position, r64 y_position) {

        auto window = get_window(handle);
        if (!window)
            return;

        if (window->input)
            window->input->mouse_move.add({ window->get_id(), { x_position, y_position } });
    });

    glfwSetCursorEnterCallback(handle, [](GLFWwindow* handle, i32 entered) {

        auto window = get_window(handle);
        if (!window)
            return;

        if (window->input)
            window->input->mouse_active.add({ window->get_id(), entered > 0 });
    });
}

template <int attr>
static int is_attribute_set(GLFWwindow* handle) { return glfwGetWindowAttrib(handle, attr); }

template <int attr>
static bool is_bool_attribute_set(GLFWwindow* handle) { return is_attribute_set<attr>(handle) == 1; }

void window::set_position(i32 x, i32 y) { glfwSetWindowPos(handle, x, y); }

void window::get_position(i32& x, i32& y) const { glfwGetWindowPos(handle, &x, &y); }

void window::set_size(ui32 w, ui32 h) { glfwSetWindowSize(handle, w, h); }

void window::get_size(ui32& w, ui32& h) const { glfwGetWindowSize(handle, (i32*)&w, (i32*)&h); }

void window::get_framebuffer_size(ui32& w, ui32& h) const { glfwGetFramebufferSize(handle, (i32*)&w, (i32*)&h); }

void window::set_mouse_position(r64 x, r64 y) { glfwSetCursorPos(handle, x, y); }

void window::get_mouse_position(r64& x, r64& y) const { glfwGetCursorPos(handle, &x, &y); }

mouse_position window::get_mouse_position() const {

    mouse_position result;
    get_mouse_position(result.x, result.y);

    return result;
}

void window::hide_mouse_cursor() { glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); }

void window::show_mouse_cursor() { glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }

float window::get_aspect_ratio() const { return framebuffer_height != 0 ? 
                                                to_r32(framebuffer_width) / to_r32(framebuffer_height) : 0.f; }

void window::show() { glfwShowWindow(handle); }

void window::hide() { glfwHideWindow(handle); }

bool window::visible() const { return is_bool_attribute_set<GLFW_VISIBLE>(handle); }

void window::iconify() { glfwIconifyWindow(handle); }

bool window::iconified() const { return is_bool_attribute_set<GLFW_ICONIFIED>(handle); }

void window::restore() { glfwRestoreWindow(handle); }

void window::maximize() { glfwMaximizeWindow(handle); }

bool window::maximized() const { return is_bool_attribute_set<GLFW_MAXIMIZED>(handle); }

void window::focus() { glfwFocusWindow(handle); }

bool window::focused() const { return is_bool_attribute_set<GLFW_FOCUSED>(handle); }

bool window::hovered() const { return is_bool_attribute_set<GLFW_HOVERED>(handle); }

bool window::resizable() const { return is_bool_attribute_set<GLFW_RESIZABLE>(handle); }

void window::set_resizable(bool value) { glfwSetWindowAttrib(handle, GLFW_RESIZABLE, value); }

bool window::decorated() const { return is_bool_attribute_set<GLFW_DECORATED>(handle); }

void window::set_decorated(bool value) { glfwSetWindowAttrib(handle, GLFW_DECORATED, value); }

bool window::floating() const { return is_bool_attribute_set<GLFW_FLOATING>(handle); }

void window::set_floating(bool value) { glfwSetWindowAttrib(handle, GLFW_FLOATING, value); }

window* window::get_window(GLFWwindow* handle) { return static_cast<window*>(glfwGetWindowUserPointer(handle)); }

bool window::has_close_request() const { return glfwWindowShouldClose(handle) == 1; }

VkSurfaceKHR window::create_surface() { return lava::create_surface(handle); }

void window::set_icon(data_ptr data, uv2 size) {

    GLFWimage images[1];
    images[0].width = size.x;
    images[0].height = size.y;
    images[0].pixels = (uc8*)data;

    glfwSetWindowIcon(handle, 1, images);
}

} // lava

VkSurfaceKHR lava::create_surface(GLFWwindow* window) {

    VkSurfaceKHR surface = 0;
    if (failed(glfwCreateWindowSurface(instance::get(), window, memory::alloc(), &surface)))
        return 0;

    return surface;
}
