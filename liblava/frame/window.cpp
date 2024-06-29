/**
 * @file         liblava/frame/window.cpp
 * @brief        Window
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/frame/window.hpp"
#include "liblava/base/device.hpp"
#include "liblava/base/instance.hpp"
#include "liblava/util/log.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace lava {

/// Save title format
constexpr name _fmt_save_title_ = "{} [{}]";

/**
 * @brief Set window attribute
 * @tparam attr     Attribute to set
 * @param handle    Window handle
 * @return int      Result
 */
template <int attr>
int attribute_set(GLFWwindow* handle) {
    return glfwGetWindowAttrib(handle, attr);
}

/**
 * @brief Check attribute of window
 * @tparam attr     Attribute to check
 * @param handle    Window handle
 * @return Attribute is set or not
 */
template <int attr>
bool bool_attribute_set(GLFWwindow* handle) {
    return attribute_set<attr>(handle) == 1;
}

//-----------------------------------------------------------------------------
bool window::create(state::optional state) {
    auto monitor = glfwGetPrimaryMonitor();
    auto mode = glfwGetVideoMode(monitor);

    string default_title = m_title;
    if (m_save_title_active)
        default_title = fmt::format(_fmt_save_title_,
                                    m_title, m_save_name);

    if (state) {
        m_fullscreen_active = state->fullscreen;

        if (state->monitor != 0) {
            auto monitor_count = 0;
            auto monitors = glfwGetMonitors(&monitor_count);

            if (state->monitor < monitor_count) {
                monitor = monitors[state->monitor];
                mode = glfwGetVideoMode(monitor);
            }
        }

        if (state->fullscreen) {
            m_handle = glfwCreateWindow(mode->width, mode->height,
                                        str(default_title), monitor, nullptr);
            if (!m_handle) {
                logger()->error("create fullscreen window (state)");
                return false;
            }
        } else {
            m_handle = glfwCreateWindow(state->width, state->height,
                                        str(default_title), nullptr, nullptr);
            if (!m_handle) {
                logger()->error("create window (state)");
                return false;
            }

            glfwSetWindowPos(m_handle, state->x, state->y);
        }

        m_pos_x = state->x;
        m_pos_y = state->y;
        m_width = state->width;
        m_height = state->height;

        set_floating(state->floating);
        set_resizable(state->resizable);
        set_decorated(state->decorated);

        if (state->maximized)
            maximize();
    } else {
        m_pos_x = mode->width / 4;
        m_pos_y = mode->height / 4;
        m_width = mode->width / 2;
        m_height = mode->height / 2;

        if (m_fullscreen_active) {
            m_handle = glfwCreateWindow(mode->width, mode->height,
                                        str(default_title), monitor, nullptr);
            if (!m_handle) {
                logger()->error("create fullscreen window");
                return false;
            }
        } else {
            m_handle = glfwCreateWindow(m_width, m_height,
                                        str(default_title), nullptr, nullptr);
            if (!m_handle) {
                logger()->error("create window");
                return false;
            }

            glfwSetWindowPos(m_handle, m_pos_x, m_pos_y);
        }
    }

    m_switch_mode_request_active = false;
    handle_message();

    get_framebuffer_size(m_framebuffer_width, m_framebuffer_height);

    return true;
}

//-----------------------------------------------------------------------------
void window::destroy() {
    m_input = nullptr;

    glfwDestroyWindow(m_handle);
    m_handle = nullptr;
}

//-----------------------------------------------------------------------------
window::state window::get_state() const {
    window::state state;

    if (fullscreen() || iconified() || maximized()) {
        state.x = m_pos_x;
        state.y = m_pos_y;
        state.width = m_width;
        state.height = m_height;
    } else {
        get_position(state.x, state.y);
        get_size(state.width, state.height);
    }

    state.fullscreen = fullscreen();
    state.floating = floating();
    state.resizable = resizable();
    state.decorated = decorated();
    state.maximized = maximized();

    state.monitor = detect_monitor();

    return state;
}

//-----------------------------------------------------------------------------
void window::set_state(window::state& state) {
    if (state.fullscreen) {
        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);

        if (state.monitor != 0) {
            auto monitor_count = 0;
            auto monitors = glfwGetMonitors(&monitor_count);

            if (state.monitor < monitor_count) {
                monitor = monitors[state.monitor];
                mode = glfwGetVideoMode(monitor);
            }
        }
        auto monitor_pos_x = 0;
        auto monitor_pos_y = 0;
        glfwGetMonitorPos(monitor, &monitor_pos_x, &monitor_pos_y);
        glfwSetWindowMonitor(m_handle,
                             monitor,
                             monitor_pos_x,
                             monitor_pos_y,
                             mode->width,
                             mode->height,
                             GLFW_DONT_CARE);
    } else {
        glfwSetWindowPos(m_handle, state.x, state.y);
        glfwSetWindowSize(m_handle, state.width, state.height);
    }

    m_fullscreen_active = state.fullscreen;

    m_pos_x = state.x;
    m_pos_y = state.y;
    m_width = state.width;
    m_height = state.height;

    set_floating(state.floating);
    set_resizable(state.resizable);
    set_decorated(state.decorated);

    if (state.maximized)
        maximize();
}

//-----------------------------------------------------------------------------
void window::set_title(string_ref text) {
    m_title = text;

    if (!m_handle)
        return;

    if (m_save_title_active)
        glfwSetWindowTitle(m_handle,
                           str(fmt::format(_fmt_save_title_,
                                           m_title, m_save_name)));
    else
        glfwSetWindowTitle(m_handle, str(m_title));
}

bool window::switch_mode(state::optional state) {
    destroy();

    return create(state);
}

//-----------------------------------------------------------------------------
void window::handle_mouse_message() {
    glfwSetMouseButtonCallback(m_handle,
                               [](GLFWwindow* handle,
                                  i32 button, i32 a, i32 m) {
                                   auto window = get_window(handle);
                                   if (!window)
                                       return;

                                   if (window->m_input)
                                       window->m_input->mouse_button.add(
                                           {window->get_id(),
                                            mouse_button(button),
                                            action(a),
                                            mod(m)});
                               });

    glfwSetCursorPosCallback(m_handle,
                             [](GLFWwindow* handle, r64 x_position, r64 y_position) {
                                 auto window = get_window(handle);
                                 if (!window)
                                     return;

                                 if (window->m_input)
                                     window->m_input->mouse_move.add(
                                         {window->get_id(),
                                          {x_position, y_position}});
                             });

    glfwSetCursorEnterCallback(m_handle, [](GLFWwindow* handle, i32 entered) {
        auto window = get_window(handle);
        if (!window)
            return;

        if (window->m_input)
            window->m_input->mouse_active.add({window->get_id(),
                                               entered > 0});
    });
}

//-----------------------------------------------------------------------------
void window::handle_message() {
    glfwSetWindowUserPointer(m_handle, this);

    glfwSetFramebufferSizeCallback(m_handle, [](GLFWwindow* handle, i32 w, i32 h) {
        auto window = get_window(handle);
        if (!window)
            return;

        window->m_framebuffer_width = to_ui32(w);
        window->m_framebuffer_height = to_ui32(h);
        window->m_resize_request_active = true;

        if (!window->fullscreen() && !window->iconified() && !window->maximized())
            window->update_state();
    });

    glfwSetKeyCallback(m_handle,
                       [](GLFWwindow* handle,
                          i32 k, i32 scancode,
                          i32 a, i32 m) {
                           auto window = get_window(handle);
                           if (!window)
                               return;

                           window->m_input->key.add(
                               {window->get_id(),
                                key(k),
                                action(a),
                                mod(m),
                                scancode});
                       });

    glfwSetScrollCallback(m_handle,
                          [](GLFWwindow* handle, r64 x_offset, r64 y_offset) {
                              auto window = get_window(handle);
                              if (!window)
                                  return;

                              if (window->m_input)
                                  window->m_input->scroll.add(
                                      {window->get_id(), x_offset, y_offset});
                          });

    handle_mouse_message();

    glfwSetDropCallback(m_handle,
                        [](GLFWwindow* handle, i32 amt, const char** paths) {
                            auto window = get_window(handle);
                            if (!window)
                                return;

                            if (window->m_input)
                                window->m_input->path_drop.add(
                                    {window->get_id(),
                                     {paths, paths + amt}});
                        });
}

//-----------------------------------------------------------------------------
void window::set_position(i32 x, i32 y) {
    glfwSetWindowPos(m_handle, x, y);
}

//-----------------------------------------------------------------------------
void window::get_position(i32& x, i32& y) const {
    glfwGetWindowPos(m_handle, &x, &y);
}

//-----------------------------------------------------------------------------
void window::set_size(ui32 w, ui32 h) {
    glfwSetWindowSize(m_handle, w, h);
}

//-----------------------------------------------------------------------------
void window::get_size(ui32& w, ui32& h) const {
    glfwGetWindowSize(m_handle, (i32*)&w, (i32*)&h);
}

//-----------------------------------------------------------------------------
void window::get_framebuffer_size(ui32& w, ui32& h) const {
    glfwGetFramebufferSize(m_handle, (i32*)&w, (i32*)&h);
}

//-----------------------------------------------------------------------------
uv2 window::get_size() const {
    uv2 size;
    get_size(size.x, size.y);
    return size;
}

//-----------------------------------------------------------------------------
uv2 window::get_framebuffer_size() const {
    uv2 size;
    get_framebuffer_size(size.x, size.y);
    return size;
}

//-----------------------------------------------------------------------------
void window::set_mouse_position(r64 x, r64 y) {
    glfwSetCursorPos(m_handle, x, y);
}

//-----------------------------------------------------------------------------
void window::get_mouse_position(r64& x, r64& y) const {
    glfwGetCursorPos(m_handle, &x, &y);
}

//-----------------------------------------------------------------------------
mouse_position window::get_mouse_position() const {
    mouse_position result;
    get_mouse_position(result.x, result.y);

    return result;
}

//-----------------------------------------------------------------------------
v2 window::get_content_scale() const {
    v2 result;
    glfwGetWindowContentScale(m_handle, &result.x, &result.y);

    return result;
}

//-----------------------------------------------------------------------------
void window::hide_mouse_cursor() {
    glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

//-----------------------------------------------------------------------------
void window::show_mouse_cursor() {
    glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

//-----------------------------------------------------------------------------
r32 window::get_aspect_ratio() const {
    return m_framebuffer_height != 0
               ? to_r32(m_framebuffer_width) / to_r32(m_framebuffer_height)
               : 0.f;
}

//-----------------------------------------------------------------------------
void window::show() {
    glfwShowWindow(m_handle);
}

//-----------------------------------------------------------------------------
void window::hide() {
    glfwHideWindow(m_handle);
}

//-----------------------------------------------------------------------------
bool window::visible() const {
    return bool_attribute_set<GLFW_VISIBLE>(m_handle);
}

void window::iconify() {
    glfwIconifyWindow(m_handle);
}

//-----------------------------------------------------------------------------
bool window::iconified() const {
    return bool_attribute_set<GLFW_ICONIFIED>(m_handle);
}

//-----------------------------------------------------------------------------
void window::restore() {
    glfwRestoreWindow(m_handle);
}

//-----------------------------------------------------------------------------
void window::maximize() {
    glfwMaximizeWindow(m_handle);
}

//-----------------------------------------------------------------------------
bool window::maximized() const {
    return bool_attribute_set<GLFW_MAXIMIZED>(m_handle);
}

//-----------------------------------------------------------------------------
void window::focus() {
    glfwFocusWindow(m_handle);
}

//-----------------------------------------------------------------------------
bool window::focused() const {
    return bool_attribute_set<GLFW_FOCUSED>(m_handle);
}

//-----------------------------------------------------------------------------
bool window::hovered() const {
    return bool_attribute_set<GLFW_HOVERED>(m_handle);
}

//-----------------------------------------------------------------------------
bool window::resizable() const {
    return bool_attribute_set<GLFW_RESIZABLE>(m_handle);
}

//-----------------------------------------------------------------------------
void window::set_resizable(bool value) {
    glfwSetWindowAttrib(m_handle, GLFW_RESIZABLE, value);
}

//-----------------------------------------------------------------------------
bool window::decorated() const {
    return bool_attribute_set<GLFW_DECORATED>(m_handle);
}

//-----------------------------------------------------------------------------
void window::set_decorated(bool value) {
    glfwSetWindowAttrib(m_handle, GLFW_DECORATED, value);
}

//-----------------------------------------------------------------------------
bool window::floating() const {
    return bool_attribute_set<GLFW_FLOATING>(m_handle);
}

//-----------------------------------------------------------------------------
void window::set_floating(bool value) {
    glfwSetWindowAttrib(m_handle, GLFW_FLOATING, value);
}

//-----------------------------------------------------------------------------
bool window::close_request() const {
    return glfwWindowShouldClose(m_handle) == 1;
}

//-----------------------------------------------------------------------------
VkSurfaceKHR window::create_surface() {
    return lava::create_surface(m_handle);
}

//-----------------------------------------------------------------------------
void window::set_icon(data::c_ptr data, uv2 size) {
    GLFWimage images[1];
    images[0].width = size.x;
    images[0].height = size.y;
    images[0].pixels = (uc8*)data;

    glfwSetWindowIcon(m_handle, 1, images);
}

//-----------------------------------------------------------------------------
index window::detect_monitor() const {
    auto window_x = 0;
    auto window_y = 0;
    glfwGetWindowPos(m_handle,
                     &window_x,
                     &window_y);

    auto window_width = 0;
    auto window_height = 0;
    glfwGetWindowSize(m_handle,
                      &window_width,
                      &window_height);

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

        overlap = std::max(0, std::min(window_x + window_width,
                                       monitor_x + monitor_width)
                                  - std::max(window_x, monitor_x))
                  * std::max(0, std::min(window_y + window_height,
                                         monitor_y + monitor_height)
                                    - std::max(window_y, monitor_y));

        if (best_overlap < overlap) {
            best_overlap = overlap;
            result = i;
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
void window::center() {
    auto monitor = glfwGetPrimaryMonitor();
    auto mode = glfwGetVideoMode(monitor);

    glfwSetWindowPos(m_handle,
                     (mode->width - m_width) / 2,
                     (mode->height - m_height) / 2);
}

//-----------------------------------------------------------------------------
VkSurfaceKHR create_surface(GLFWwindow* window) {
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (failed(glfwCreateWindowSurface(instance::singleton().get(),
                                       window, memory::instance().alloc(),
                                       &surface)))
        return 0;

    return surface;
}

//-----------------------------------------------------------------------------
window* get_window(GLFWwindow* handle) {
    return static_cast<window*>(glfwGetWindowUserPointer(handle));
}

} // namespace lava
