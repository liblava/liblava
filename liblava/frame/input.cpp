// file      : liblava/frame/input.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/frame/input.hpp>

namespace lava {

void input::clear_all_events() {

    clear_key_events();
    clear_scroll_events();
    clear_mouse_button_events();
    clear_mouse_move_events();
    clear_mouse_active_events();
}

} // lava

void lava::handle_key_events(input& input) {

    for (auto& event : input.get_key_events()) {

        for (auto& listener : input.key_listeners.get_list())
            listener.second(event);

        for (auto& callback : input.get_callbacks())
            callback->on_key_event(event);
    }

    input.clear_key_events();
}

void lava::handle_scroll_events(input& input) {

    for (auto& event : input.get_scroll_events()) {

        for (auto& listener : input.scroll_listeners.get_list())
            listener.second(event);

        for (auto& callback : input.get_callbacks())
            callback->on_scroll_event(event);
    }

    input.clear_scroll_events();
}

void lava::handle_mouse_button_events(input& input) {

    for (auto& event : input.get_mouse_button_events()) {

        for (auto& listener : input.mouse_button_listeners.get_list())
            listener.second(event);

        for (auto& callback : input.get_callbacks())
            callback->on_mouse_button_event(event);
    }

    input.clear_mouse_button_events();
}

void lava::handle_mouse_move_events(input& input) {

    for (auto& event : input.get_mouse_move_events()) {

        for (auto& listener : input.mouse_move_listeners.get_list())
            listener.second(event);

        for (auto& callback : input.get_callbacks())
            callback->on_mouse_move_event(event);
    }

    input.clear_mouse_move_events();
}

void lava::handle_mouse_active_events(input& input) {

    for (auto& event : input.get_mouse_active_events()) {

        for (auto& listener : input.mouse_active_listeners.get_list())
            listener.second(event);

        for (auto& callback : input.get_callbacks())
            callback->on_mouse_active_event(event);
    }

    input.clear_mouse_active_events();
}

void lava::handle_events(input& input) {

    handle_key_events(input);
    handle_scroll_events(input);
    handle_mouse_button_events(input);
    handle_mouse_move_events(input);
    handle_mouse_active_events(input);
}
