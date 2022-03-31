/**
 * @file         liblava/frame/input.cpp
 * @brief        Input handling
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/frame/input.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace lava {

/**
 * @brief Handle events
 *
 * @tparam T                Type of event
 *
 * @param events            Events to handle
 * @param input_callback    Input callback
 */
template<typename T>
void _handle_events(input_events<T>& events, input_callback::func<T> input_callback) {
    for (auto const& event : events) {
        auto handled = false;

        for (auto const& [id, listener] : events.listeners.get_list()) {
            if (listener(event)) {
                handled = true;
                break;
            }
        }

        if (handled)
            continue;

        if (input_callback)
            input_callback(event);
    }

    events.clear();
}

//-----------------------------------------------------------------------------
void input::handle_mouse_events() {
    _handle_events<mouse_move_event>(mouse_move, [&](auto& event) {
        for (auto const& callback : callbacks)
            if (callback->on_mouse_move_event)
                if (callback->on_mouse_move_event(event))
                    return true;

        return false;
    });

    _handle_events<mouse_button_event>(mouse_button, [&](auto& event) {
        for (auto const& callback : callbacks)
            if (callback->on_mouse_button_event)
                if (callback->on_mouse_button_event(event))
                    return true;

        return false;
    });

    _handle_events<mouse_active_event>(mouse_active, [&](auto& event) {
        for (auto const& callback : callbacks)
            if (callback->on_mouse_active_event)
                if (callback->on_mouse_active_event(event))
                    return true;

        return false;
    });
}

//-----------------------------------------------------------------------------
void input::handle_events() {
    _handle_events<key_event>(key, [&](auto& event) {
        for (auto const& callback : callbacks)
            if (callback->on_key_event)
                if (callback->on_key_event(event))
                    return true;

        return false;
    });

    _handle_events<scroll_event>(scroll, [&](auto& event) {
        for (auto const& callback : callbacks)
            if (callback->on_scroll_event)
                if (callback->on_scroll_event(event))
                    return true;

        return false;
    });

    handle_mouse_events();

    _handle_events<path_drop_event>(path_drop, [&](auto& event) {
        for (auto const& callback : callbacks)
            if (callback->on_path_drop_event)
                if (callback->on_path_drop_event(event))
                    return true;

        return false;
    });
}

} // namespace lava
