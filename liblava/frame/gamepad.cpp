/**
 * @file         liblava/frame/gamepad.cpp
 * @brief        Gamepad manager
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/frame/gamepad.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace lava {

//-----------------------------------------------------------------------------
gamepad::gamepad(gamepad_id_ref pad_id)
: m_pad_id(pad_id) {
    if (ready())
        update();
}

//-----------------------------------------------------------------------------
name gamepad::get_name() const {
    return glfwGetGamepadName(to_i32(m_pad_id));
}

//-----------------------------------------------------------------------------
bool gamepad::ready() const {
    return glfwJoystickPresent(to_i32(m_pad_id));
}

//-----------------------------------------------------------------------------
bool gamepad::update() {
    return glfwGetGamepadState(to_i32(m_pad_id), (GLFWgamepadstate*)&m_state)
           == GLFW_TRUE;
}

//-----------------------------------------------------------------------------
gamepad_manager::gamepad_manager() {
    glfwSetJoystickCallback([](int pad_id, int e) {
        for (auto const& [func_id, event] : gamepad_manager::singleton().m_map)
            if (event(gamepad(gamepad_id(pad_id)), e == GLFW_CONNECTED))
                break;
    });
}

//-----------------------------------------------------------------------------
id gamepad_manager::add(listener_func event) {
    auto id = ids::instance().next();
    m_map.emplace(id, event);
    return id;
}

//-----------------------------------------------------------------------------
void gamepad_manager::remove(id::ref func_id) {
    if (!m_map.count(func_id))
        return;

    m_map.erase(func_id);
}

//-----------------------------------------------------------------------------
gamepad::list gamepads() {
    gamepad::list result;

    for (auto pad_id = GLFW_JOYSTICK_1; pad_id < GLFW_JOYSTICK_LAST; ++pad_id) {
        if (glfwJoystickIsGamepad(pad_id))
            result.emplace_back(gamepad_id(pad_id));
    }

    return result;
}

} // namespace lava
