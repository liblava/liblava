// file      : liblava/frame/input.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#include <liblava/frame/input.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace lava {

    template<typename T>
    void _handle_events(input_events<T>& events, input_callback::func<T> input_callback) {
        for (auto& event : events) {
            auto handled = false;

            for (auto& listener : events.listeners.get_list()) {
                if (listener.second(event)) {
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

    void input::handle_mouse_events() {
        _handle_events<mouse_move_event>(mouse_move, [&](auto& event) {
            for (auto& callback : callbacks)
                if (callback->on_mouse_move_event)
                    if (callback->on_mouse_move_event(event))
                        return true;

            return false;
        });

        _handle_events<mouse_button_event>(mouse_button, [&](auto& event) {
            for (auto& callback : callbacks)
                if (callback->on_mouse_button_event)
                    if (callback->on_mouse_button_event(event))
                        return true;

            return false;
        });

        _handle_events<mouse_active_event>(mouse_active, [&](auto& event) {
            for (auto& callback : callbacks)
                if (callback->on_mouse_active_event)
                    if (callback->on_mouse_active_event(event))
                        return true;

            return false;
        });
    }

    void input::handle_events() {
        _handle_events<key_event>(key, [&](auto& event) {
            for (auto& callback : callbacks)
                if (callback->on_key_event)
                    if (callback->on_key_event(event))
                        return true;

            return false;
        });

        _handle_events<scroll_event>(scroll, [&](auto& event) {
            for (auto& callback : callbacks)
                if (callback->on_scroll_event)
                    if (callback->on_scroll_event(event))
                        return true;

            return false;
        });

        handle_mouse_events();

        _handle_events<path_drop_event>(path_drop, [&](auto& event) {
            for (auto& callback : callbacks)
                if (callback->on_path_drop_event)
                    if (callback->on_path_drop_event(event))
                        return true;

            return false;
        });
    }

    gamepad::gamepad(gamepad_id id)
    : id(id) {
        if (ready())
            update();
    }

    name gamepad::get_name() const {
        return glfwGetGamepadName(to_i32(id));
    }

    bool gamepad::ready() const {
        return glfwJoystickIsGamepad(to_i32(id));
    }

    bool gamepad::update() {
        return glfwGetGamepadState(to_i32(id), (GLFWgamepadstate*) &state) == GLFW_TRUE;
    }

    gamepad_manager::gamepad_manager() {
        glfwSetJoystickCallback([](int jid, int e) {
            for (auto& event : instance().map)
                if (event.second(gamepad(gamepad_id(jid)), e == GLFW_CONNECTED))
                    break;
        });
    }

    id gamepad_manager::add(listener_func event) {
        auto id = ids::next();

        instance().map.emplace(id, event);

        return id;
    }

    void gamepad_manager::remove(id::ref id) {
        if (!instance().map.count(id))
            return;

        instance().map.erase(id);

        ids::free(id);
    }

} // namespace lava

lava::gamepad::list lava::gamepads() {
    gamepad::list result;

    for (auto id = GLFW_JOYSTICK_1; id < GLFW_JOYSTICK_LAST; ++id) {
        if (glfwJoystickIsGamepad(id))
            result.emplace_back(gamepad_id(id));
    }

    return result;
}
