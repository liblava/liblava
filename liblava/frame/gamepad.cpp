/**
 * @file         liblava/frame/gamepad.cpp
 * @brief        Gamepad manager
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/frame/gamepad.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace lava {

//-----------------------------------------------------------------------------
gamepad::gamepad(gamepad_id_ref id)
: id(id) {
    if (ready())
        update();
}

//-----------------------------------------------------------------------------
name gamepad::get_name() const {
    return glfwGetGamepadName(to_i32(id));
}

//-----------------------------------------------------------------------------
bool gamepad::ready() const {
    return glfwJoystickIsGamepad(to_i32(id));
}

//-----------------------------------------------------------------------------
bool gamepad::update() {
    return glfwGetGamepadState(to_i32(id), (GLFWgamepadstate*) &state) == GLFW_TRUE;
}

//-----------------------------------------------------------------------------
gamepad_manager::gamepad_manager() {
    glfwSetJoystickCallback([](int jid, int e) {
        for (auto const& [id, event] : instance().map)
            if (event(gamepad(gamepad_id(jid)), e == GLFW_CONNECTED))
                break;
    });
}

//-----------------------------------------------------------------------------
id gamepad_manager::add(listener_func event) {
    auto id = ids::next();

    instance().map.emplace(id, event);

    return id;
}

//-----------------------------------------------------------------------------
void gamepad_manager::remove(id::ref id) {
    if (!instance().map.count(id))
        return;

    instance().map.erase(id);

    ids::free(id);
}

//-----------------------------------------------------------------------------
gamepad::list gamepads() {
    gamepad::list result;

    for (auto id = GLFW_JOYSTICK_1; id < GLFW_JOYSTICK_LAST; ++id) {
        if (glfwJoystickIsGamepad(id))
            result.emplace_back(gamepad_id(id));
    }

    return result;
}

} // namespace lava
