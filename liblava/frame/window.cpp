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

    auto monitor = glfwGetPrimaryMonitor();
    auto mode = glfwGetVideoMode(monitor);

    string default_title = title;
    if (debug_title_active)
        default_title = fmt::format("%s [%s]", str(title), str(save_name));

    if (state) {

        fullscreen_active = state->fullscreen;

        if (state->monitor != 0) {

            auto monitor_count = 0;
            auto monitors = glfwGetMonitors(&monitor_count);

            if (state->monitor < monitor_count) {

                monitor = monitors[state->monitor];
                mode = glfwGetVideoMode(monitor);
            }
        }

        if (state->fullscreen) {

            handle = glfwCreateWindow(mode->width, mode->height, str(default_title), monitor, nullptr);
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

        if (fullscreen_active) {

            handle = glfwCreateWindow(mode->width, mode->height, str(default_title), monitor, nullptr);
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

    switch_mode_request_active = false;
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

    if (fullscreen() || iconified() || maximized()) {

        state.x = pos_x;
        state.y = pos_y;
        state.width = width;
        state.height = height;
    }
    else {

        get_position(state.x, state.y);
        get_size(state.width, state.height);
    }
    
    state.fullscreen = fullscreen();
    state.floating = floating();
    state.resizable = resizable();
    state.decorated = decorated();
    state.maximized = maximized();

    state.monitor = get_monitor();

    return state;
}

void window::set_title(name text) {

    title = text;

    if (!handle)
        return;

    if (debug_title_active)
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

    glfwSetFramebufferSizeCallback(handle, [](GLFWwindow* handle, i32 w, i32 h) {

        auto window = get_window(handle);
        if (!window)
            return;

        window->framebuffer_width = to_ui32(w);
        window->framebuffer_height = to_ui32(h);
        window->resize_request_active = true;

        if (!window->fullscreen() && !window->iconified() && !window->maximized())
            window->update_state();
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
static int attribute_set(GLFWwindow* handle) { return glfwGetWindowAttrib(handle, attr); }

template <int attr>
static bool bool_attribute_set(GLFWwindow* handle) { return attribute_set<attr>(handle) == 1; }

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

bool window::visible() const { return bool_attribute_set<GLFW_VISIBLE>(handle); }

void window::iconify() { glfwIconifyWindow(handle); }

bool window::iconified() const { return bool_attribute_set<GLFW_ICONIFIED>(handle); }

void window::restore() { glfwRestoreWindow(handle); }

void window::maximize() { glfwMaximizeWindow(handle); }

bool window::maximized() const { return bool_attribute_set<GLFW_MAXIMIZED>(handle); }

void window::focus() { glfwFocusWindow(handle); }

bool window::focused() const { return bool_attribute_set<GLFW_FOCUSED>(handle); }

bool window::hovered() const { return bool_attribute_set<GLFW_HOVERED>(handle); }

bool window::resizable() const { return bool_attribute_set<GLFW_RESIZABLE>(handle); }

void window::set_resizable(bool value) { glfwSetWindowAttrib(handle, GLFW_RESIZABLE, value); }

bool window::decorated() const { return bool_attribute_set<GLFW_DECORATED>(handle); }

void window::set_decorated(bool value) { glfwSetWindowAttrib(handle, GLFW_DECORATED, value); }

bool window::floating() const { return bool_attribute_set<GLFW_FLOATING>(handle); }

void window::set_floating(bool value) { glfwSetWindowAttrib(handle, GLFW_FLOATING, value); }

window* window::get_window(GLFWwindow* handle) { return static_cast<window*>(glfwGetWindowUserPointer(handle)); }

bool window::close_request() const { return glfwWindowShouldClose(handle) == 1; }

VkSurfaceKHR window::create_surface() { return lava::create_surface(handle); }

void window::set_icon(data_ptr data, uv2 size) {

    GLFWimage images[1];
    images[0].width = size.x;
    images[0].height = size.y;
    images[0].pixels = (uc8*)data;

    glfwSetWindowIcon(handle, 1, images);
}

index window::get_monitor() const {

    auto window_x = 0;
    auto window_y = 0;
    glfwGetWindowPos(handle, &window_x, &window_y);

    auto window_width = 0;
    auto window_height = 0;
    glfwGetWindowSize(handle, &window_width, &window_height);

    auto monitor_count = 0;
    auto monitors = glfwGetMonitors(&monitor_count);

    index result = 0;

    auto overlap = 0;
    auto best_overlap = 0;

    for (auto i = 0u; i < monitor_count; ++i) {

        auto monitor_x = 0;
        auto monitor_y = 0;
        glfwGetMonitorPos(monitors[i], &monitor_x, &monitor_y);

        auto mode = glfwGetVideoMode(monitors[i]);
        auto monitor_width = mode->width;
        auto monitor_height = mode->height;

        overlap = std::max(0, std::min(window_x + window_width, monitor_x + monitor_width) - 
                              std::max(window_x, monitor_x)) *
                  std::max(0, std::min(window_y + window_height, monitor_y + monitor_height) - 
                              std::max(window_y, monitor_y));

        if (best_overlap < overlap) {

            best_overlap = overlap;
            result = i;
        }
    }

    return result;
}

void window::center() {

    auto monitor = glfwGetPrimaryMonitor();
    auto mode = glfwGetVideoMode(monitor);

    glfwSetWindowPos(handle, (mode->width - width) / 2, (mode->height - height) / 2);
}

void to_json(json& j, window::state const& w) {

    j = json{ { _x_, w.x }, { _y_, w.y }, { _width_, w.width }, { _height_, w.height },
              { _fullscreen_, w.fullscreen }, { _floating_, w.floating }, { _resizable_, w.resizable },
              { _decorated_, w.decorated }, { _maximized_, w.maximized }, { _monitor_, w.monitor } };
}

void from_json(json const& j, window::state& w) {

    if (j.count(_x_))
        w.x = j.at(_x_).get<int>();
    if (j.count(_y_))
        w.y = j.at(_y_).get<int>();
    if (j.count(_width_))
        w.width = j.at(_width_).get<int>();
    if (j.count(_height_))
        w.height = j.at(_height_).get<int>();
    if (j.count(_fullscreen_))
        w.fullscreen = j.at(_fullscreen_).get<bool>();
    if (j.count(_floating_))
        w.floating = j.at(_floating_).get<bool>();
    if (j.count(_resizable_))
        w.resizable = j.at(_resizable_).get<bool>();
    if (j.count(_decorated_))
        w.decorated = j.at(_decorated_).get<bool>();
    if (j.count(_maximized_))
        w.maximized = j.at(_maximized_).get<bool>();
    if (j.count(_monitor_))
        w.monitor = j.at(_monitor_).get<int>();
}

} // lava

bool lava::window_file() {

    return file_system::exists(_window_file_);
}

lava::window::state::optional lava::load_window_state(name save_name) {

    if (!window_file())
        return {};

    window::state window_state;
    if (!load_window_file(window_state, save_name))
        return {};

    return window_state;
}

bool lava::load_window_file(window::state& state, name save_name) {

    scope_data data;
    if (!load_file_data(_window_file_, data))
        return false;

    auto j = json::parse({ data.ptr, data.size });

    if (!j.count(save_name))
        return false;

    log()->trace("load window {}", str(j.dump()));

    state = j[save_name];
    return true;
}

void lava::save_window_file(window::ref window) {

    window::state state = window.get_state();
    auto index = window.get_save_name();

    json j;

    scope_data data;
    if (load_file_data(_window_file_, data)) {

        j = json::parse({ data.ptr, data.size });

        json d;
        d[index] = state;

        j.merge_patch(d);
    }
    else {

        j[index] = state;
    }

    file file(str(_window_file_), true);
    if (!file.opened()) {

        log()->error("save window {}", str(j.dump()));
        return;
    }

    auto jString = j.dump(4);

    file.write(jString.data(), jString.size());

    log()->trace("save window {}", str(j.dump()));
}

VkSurfaceKHR lava::create_surface(GLFWwindow* window) {

    VkSurfaceKHR surface = 0;
    if (failed(glfwCreateWindowSurface(instance::get(), window, memory::alloc(), &surface)))
        return 0;

    return surface;
}
