/**
 * @file         liblava/frame/input.cpp
 * @brief        Input handling
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/frame/input.hpp"
#include "liblava/util/log.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace lava {

/**
 * @brief Handle events
 * @tparam T                Type of event
 * @param events            Events to handle
 * @param input_callback    Input callback
 */
template<typename T>
void _handle_events(input_events<T>& events,
                    input_callback::func<T> input_callback) {
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

//-----------------------------------------------------------------------------
i32 get_scancode(key key) {
    return glfwGetKeyScancode(to_i32(key));
}

//-----------------------------------------------------------------------------
string to_string(key key) {
    switch (key) {
    case key::unknown:
        return "unknown";
    case key::space:
        return "space";
    case key::apostrophe:
        return "apostrophe";
    case key::comma:
        return "comma";
    case key::minus:
        return "minus";
    case key::period:
        return "period";
    case key::slash:
        return "slash";
    case key::_0:
        return "0";
    case key::_1:
        return "1";
    case key::_2:
        return "2";
    case key::_3:
        return "3";
    case key::_4:
        return "4";
    case key::_5:
        return "5";
    case key::_6:
        return "6";
    case key::_7:
        return "7";
    case key::_8:
        return "8";
    case key::_9:
        return "9";
    case key::semicolon:
        return "semicolon";
    case key::equal:
        return "equal";
    case key::a:
        return "a";
    case key::b:
        return "b";
    case key::c:
        return "c";
    case key::d:
        return "d";
    case key::e:
        return "e";
    case key::f:
        return "f";
    case key::g:
        return "g";
    case key::h:
        return "h";
    case key::i:
        return "i";
    case key::j:
        return "j";
    case key::k:
        return "k";
    case key::l:
        return "l";
    case key::m:
        return "m";
    case key::n:
        return "n";
    case key::o:
        return "o";
    case key::p:
        return "p";
    case key::q:
        return "q";
    case key::r:
        return "r";
    case key::s:
        return "s";
    case key::t:
        return "t";
    case key::u:
        return "u";
    case key::v:
        return "v";
    case key::w:
        return "w";
    case key::x:
        return "x";
    case key::y:
        return "y";
    case key::z:
        return "z";
    case key::left_bracket:
        return "left_bracket";
    case key::backslash:
        return "backslash";
    case key::right_bracket:
        return "right_bracket";
    case key::grave_accent:
        return "grave_accent";
    case key::world_1:
        return "world_1";
    case key::world_2:
        return "world_2";
    case key::escape:
        return "escape";
    case key::enter:
        return "enter";
    case key::tab:
        return "tab";
    case key::backspace:
        return "backspace";
    case key::insert:
        return "insert";
    case key::del:
        return "del";
    case key::right:
        return "right";
    case key::left:
        return "left";
    case key::down:
        return "down";
    case key::up:
        return "up";
    case key::page_up:
        return "page_up";
    case key::page_down:
        return "page_down";
    case key::home:
        return "home";
    case key::end:
        return "end";
    case key::caps_lock:
        return "caps_lock";
    case key::scroll_lock:
        return "scroll_lock";
    case key::num_lock:
        return "num_lock";
    case key::print_screen:
        return "print_screen";
    case key::pause:
        return "pause";
    case key::f1:
        return "f1";
    case key::f2:
        return "f2";
    case key::f3:
        return "f3";
    case key::f4:
        return "f4";
    case key::f5:
        return "f5";
    case key::f6:
        return "f6";
    case key::f7:
        return "f7";
    case key::f8:
        return "f8";
    case key::f9:
        return "f9";
    case key::f10:
        return "f10";
    case key::f11:
        return "f11";
    case key::f12:
        return "f12";
    case key::f13:
        return "f13";
    case key::f14:
        return "f14";
    case key::f15:
        return "f15";
    case key::f16:
        return "f16";
    case key::f17:
        return "f17";
    case key::f18:
        return "f18";
    case key::f19:
        return "f19";
    case key::f20:
        return "f20";
    case key::f21:
        return "f21";
    case key::f22:
        return "f22";
    case key::f23:
        return "f23";
    case key::f24:
        return "f24";
    case key::f25:
        return "f25";
    case key::kp_0:
        return "kp_0";
    case key::kp_1:
        return "kp_1";
    case key::kp_2:
        return "kp_2";
    case key::kp_3:
        return "kp_3";
    case key::kp_4:
        return "kp_4";
    case key::kp_5:
        return "kp_5";
    case key::kp_6:
        return "kp_6";
    case key::kp_7:
        return "kp_7";
    case key::kp_8:
        return "kp_8";
    case key::kp_9:
        return "kp_9";
    case key::kp_decimal:
        return "kp_decimal";
    case key::kp_divide:
        return "kp_divide";
    case key::kp_multiply:
        return "kp_multiply";
    case key::kp_subtract:
        return "kp_subtract";
    case key::kp_add:
        return "kp_add";
    case key::kp_enter:
        return "kp_enter";
    case key::kp_equal:
        return "kp_equal";
    case key::left_shift:
        return "left_shift";
    case key::left_control:
        return "left_control";
    case key::left_alt:
        return "left_alt";
    case key::left_super:
        return "left_super";
    case key::right_shift:
        return "right_shift";
    case key::right_control:
        return "right_control";
    case key::right_alt:
        return "right_alt";
    case key::right_super:
        return "right_super";
    case key::menu:
        return "menu";
    default:
        return {};
    }
}

//-----------------------------------------------------------------------------
string to_string(mod m) {
    string result;

    if (check_mod(m, mod::shift))
        result += "shift ";
    if (check_mod(m, mod::control))
        result += "control ";
    if (check_mod(m, mod::alt))
        result += "alt ";
    if (check_mod(m, mod::super))
        result += "super ";
    if (check_mod(m, mod::caps_lock))
        result += "caps_lock ";
    if (check_mod(m, mod::num_lock))
        result += "num_lock ";

    lava::rtrim(result);

    return result;
}

//-----------------------------------------------------------------------------
string tooltip_list::format_string() const {
    string result;

    auto skip_first = true;
    for (auto& tooltip : tooltips) {
        if (skip_first)
            skip_first = false;
        else
            result += "\n";

        if (tooltip.mod == mod::none)
            result += fmt::format("{} = {}",
                                  tooltip.name, to_string(tooltip.key));
        else
            result += fmt::format("{} = {} + {}",
                                  tooltip.name, to_string(tooltip.mod),
                                  to_string(tooltip.key));
    }

    return result;
}

} // namespace lava
