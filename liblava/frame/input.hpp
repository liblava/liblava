// file      : liblava/frame/input.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/utils/utility.hpp>
#include <liblava/core/id.hpp>

namespace lava {

struct key_event {

    using ref = key_event const&;
    using func = std::function<void(ref)>;
    using listeners = std::map<id, func>;
    using list = std::vector<key_event>;

    id sender;

    i32 key = 0;
    i32 scancode = 0;
    i32 action = 0;
    i32 mods = 0;
};

struct scroll_offset {

    r64 x = 0.0;
    r64 y = 0.0;
};

struct scroll_event {

    using ref = scroll_event const&;
    using func = std::function<void(ref)>;
    using listeners = std::map<id, func>;
    using list = std::vector<scroll_event>;

    id sender;

    scroll_offset offset;
};

struct mouse_button_event {

    using ref = mouse_button_event const&;
    using func = std::function<void(ref)>;
    using listeners = std::map<id, func>;
    using list = std::vector<mouse_button_event>;

    id sender;

    i32 button = 0;
    i32 action = 0;
    i32 modes = 0;
};

struct mouse_position {

    r64 x = 0.0;
    r64 y = 0.0;
};

struct mouse_move_event {

    using ref = mouse_move_event const&;
    using func = std::function<void(ref)>;
    using listeners = std::map<id, func>;
    using list = std::vector<mouse_move_event>;

    id sender;

    mouse_position position;
};

struct mouse_active_event {

    using ref = mouse_active_event const&;
    using func = std::function<void(ref)>;
    using listeners = std::map<id, func>;
    using list = std::vector<mouse_active_event>;

    id sender;

    bool active = false;
};

struct input_callback {

    using list = std::vector<input_callback*>;

    key_event::func on_key_event;
    scroll_event::func on_scroll_event;
    mouse_button_event::func on_mouse_button_event;
    mouse_move_event::func on_mouse_move_event;
    mouse_active_event::func on_mouse_active_event;
};

struct input {

    void add_key_event(key_event::ref event) { key_events.push_back(event); }
    void add_scroll_event(scroll_event::ref event) { scroll_events.push_back(event); }
    void add_mouse_button_event(mouse_button_event::ref event) { mouse_button_events.push_back(event); }
    void add_mouse_move_event(mouse_move_event::ref event) { mouse_move_events.push_back(event); }
    void add_mouse_active_event(mouse_active_event::ref event) { mouse_active_events.push_back(event); }

    listeners<key_event> key_listeners;
    listeners<scroll_event> scroll_listeners;
    listeners<mouse_button_event> mouse_button_listeners;
    listeners<mouse_move_event> mouse_move_listeners;
    listeners<mouse_active_event> mouse_active_listeners;

    void add_callback(input_callback* callback) { callbacks.push_back(callback); }
    void remove_callback(input_callback* callback) { remove(callbacks, callback); }
    input_callback::list const& get_callbacks() const { return callbacks; }

    key_event::list const& get_key_events() const { return key_events; }
    scroll_event::list const& get_scroll_events() const { return scroll_events; }
    mouse_button_event::list const& get_mouse_button_events() const { return mouse_button_events; }
    mouse_move_event::list const& get_mouse_move_events() const { return mouse_move_events; }
    mouse_active_event::list const& get_mouse_active_events() const { return mouse_active_events; }

    void clear_key_events() { key_events.clear(); }
    void clear_scroll_events() { scroll_events.clear(); }
    void clear_mouse_button_events() { mouse_button_events.clear(); }
    void clear_mouse_move_events() { mouse_move_events.clear(); }
    void clear_mouse_active_events() { mouse_active_events.clear(); }

    void clear_all_events();

    mouse_position get_mouse_position() const { return _mouse_position; }
    void set_mouse_position(mouse_position const& position) { _mouse_position = position; }

private:
    mouse_position _mouse_position;

    key_event::list key_events;
    scroll_event::list scroll_events;
    mouse_button_event::list mouse_button_events;
    mouse_move_event::list mouse_move_events;
    mouse_active_event::list mouse_active_events;

    input_callback::list callbacks;
};

void handle_key_events(input& input);

void handle_scroll_events(input& input);

void handle_mouse_button_events(input& input);

void handle_mouse_move_events(input& input);

void handle_mouse_active_events(input& input);

void handle_events(input& input);

} // lava
