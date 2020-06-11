// file      : liblava/frame/input.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/util/utility.hpp>
#include <liblava/core/id.hpp>

namespace lava {

enum class key : i32 {

    unknown         = -1,

    /* printable keys */

    space           = 32,
    apostrophe      = 29, /* ' */
    comma           = 44, /* , */
    minus           = 45, /* - */
    period          = 46, /* . */
    slash           = 47, /* / */

    _0              = 48,
    _1, _2, _3, _4, _5, _6, _7, _8, _9,

    semicolon       = 59, /* ; */
    equal           = 61, /* = */

    a               = 65,
    b, c, d, e, f, g, h, i, j, k, l, m, n,
    o, p, q, r, s, t, u, v, w, x, y, z,

    left_bracket    = 91, /* [ */
    backslash       = 92, /* \ */
    right_bracket   = 93, /* ] */
    grave_accent    = 96, /* ` */

    world_1         = 161, /* non-US #1 */
    world_2         = 162, /* non-US #2 */

    /* function keys */

    escape          = 256,
    enter, tab, backspace, insert, del, /* delete */

    right, left, down, up,

    page_up, page_down, home, end,

    caps_lock       = 280,
    scroll_lock, num_lock, print_screen, pause,

    f1              = 290,
    f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14,
    f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25,

    kp_0            = 320,
    kp_1, kp_2, kp_3, kp_4, kp_5, kp_6, kp_7, kp_8, kp_9,

    kp_decimal, kp_divide, kp_multiply, kp_subtract,
    kp_add, kp_enter, kp_equal,

    left_shift      = 340,
    left_control, left_alt, left_super,

    right_shift, right_control, right_alt, right_super,

    menu,

    last = menu,
};

enum class action : type {

    release = 0, press, repeat
};

enum class mod : c8 {

    shift       = 0x01,
    control     = 0x02,
    alt         = 0x04,
    super       = 0x08,
    caps_lock   = 0x10,
    num_lock    = 0x20,
};

struct key_event {

    using ref = key_event const&;
    using func = std::function<bool(ref)>;
    using listeners = std::map<id, func>;
    using list = std::vector<key_event>;

    id sender;

    lava::key key;
    lava::action action;
    lava::mod mod;

    i32 scancode = 0;

    bool pressed(lava::key k) const { return action == action::press && key == k; }
    bool released(lava::key k) const { return action == action::release && key == k; }
    bool repeated(lava::key k) const { return action == action::repeat && key == k; }

    bool active() const { return action == action::press || action == action::repeat; }

    bool pressed(lava::key k, lava::mod m) const { return pressed(k) && mod == m; }
};

struct scroll_offset {

    r64 x = 0.0;
    r64 y = 0.0;
};

struct scroll_event {

    using ref = scroll_event const&;
    using func = std::function<bool(ref)>;
    using listeners = std::map<id, func>;
    using list = std::vector<scroll_event>;

    id sender;

    scroll_offset offset;
};

struct mouse_position {

    r64 x = 0.0;
    r64 y = 0.0;
};

struct mouse_move_event {

    using ref = mouse_move_event const&;
    using func = std::function<bool(ref)>;
    using listeners = std::map<id, func>;
    using list = std::vector<mouse_move_event>;

    id sender;

    mouse_position position;
};

enum class mouse_button : type {

    _1 = 0, _2, _3, _4, _5, _6, _7, _8,

    last = _8,

    left = _1, right = _2, middle = _3,
};

struct mouse_button_event {

    using ref = mouse_button_event const&;
    using func = std::function<bool(ref)>;
    using listeners = std::map<id, func>;
    using list = std::vector<mouse_button_event>;

    id sender;

    mouse_button button;

    lava::action action;
    lava::mod mod;

    bool pressed(mouse_button b) const { return action == action::press && button == b; }
    bool released(mouse_button b) const { return action == action::release && button == b; }
};

struct path_drop_event{
    using ref = path_drop_event const&;
    using func = std::function<bool(ref)>;
    using listeners = std::map<id, func>;
    using list = std::vector<path_drop_event>;

    id sender;

    std::vector<std::string> files;
};

struct mouse_active_event {

    using ref = mouse_active_event const&;
    using func = std::function<bool(ref)>;
    using listeners = std::map<id, func>;
    using list = std::vector<mouse_active_event>;

    id sender;

    bool active = false;
};

struct input_callback {

    using list = std::vector<input_callback*>;

    template <typename T>
    using func = std::function<bool(typename T::ref)>;

    key_event::func on_key_event;
    scroll_event::func on_scroll_event;

    mouse_move_event::func on_mouse_move_event;
    mouse_button_event::func on_mouse_button_event;
    mouse_active_event::func on_mouse_active_event;
    path_drop_event::func on_path_drop_event;
};

template <typename T>
struct input_events : T::list {

    void add(typename T::ref event) { this->push_back(event); }

    id_listeners<T> listeners;
};

struct input : id_obj {

    input_events<key_event> key;
    input_events<scroll_event> scroll;

    input_events<mouse_move_event> mouse_move;
    input_events<mouse_button_event> mouse_button;
    input_events<mouse_active_event> mouse_active;
    input_events<path_drop_event> path_drop;

    void handle_events();

    void add(input_callback* callback) { callbacks.push_back(callback); }
    void remove(input_callback* callback) { lava::remove(callbacks, callback); }

    mouse_position get_mouse_position() const { return current_position; }
    void set_mouse_position(mouse_position const& position) { current_position = position; }

private:
    mouse_position current_position;

    input_callback::list callbacks;
};

enum class gamepad_id : ui32 {

    _1 = 0, _2, _3, _4, _5, _6, _7, _8, _9,
    _10, _11, _12, _13, _14, _15, _16,

    last = _16,
};

enum class gamepad_button : type {

    a = 0, b, x, y,

    left_bumper, right_bumper,

    back, start, guide,

    left_thumb, right_thumb,

    dpad_up, dpad_right, dpad_down, dpad_left,

    last = dpad_left,

    cross = a,  circle = b,
    square = x, triangle = y,
};

enum class gamepad_axis : type {

    left_x = 0, left_y,

    right_x, right_y,

    left_trigger, right_trigger,

    last = right_trigger,
};

struct gamepad {

    using list = std::vector<gamepad>;
    using ref = gamepad const&;

    explicit gamepad(gamepad_id id);

    bool ready() const;
    bool update();

    bool pressed(gamepad_button button) const { return state.buttons[to_ui32(button)]; }
    r32 value(gamepad_axis axis) const { return state.axes[to_ui32(axis)]; }

    gamepad_id get_id() const { return id; }
    name get_name() const;

private:
    gamepad_id id;

    struct state {

        unsigned char buttons[15];
        r32 axes[6];
    };

    gamepad::state state;
};

gamepad::list gamepads();

struct gamepad_manager {

    static gamepad_manager& instance() {

        static gamepad_manager manager;
        return manager;
    }

    using listener_func = std::function<bool(gamepad, bool)>;

    id add(listener_func listener);
    void remove(id::ref id);

private:
    explicit gamepad_manager();

    using listener_map = std::map<id, listener_func>;
    listener_map map;
};

} // lava
