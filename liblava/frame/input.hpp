/**
 * @file         liblava/frame/input.hpp
 * @brief        Input handling
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/core/id.hpp>
#include <liblava/util/utility.hpp>

namespace lava {

/**
 * @brief Input keys
 */
enum class key : i32 {
    unknown = -1,

    /* printable keys */

    space = 32,
    apostrophe = 29, /* ' */
    comma = 44, /* , */
    minus = 45, /* - */
    period = 46, /* . */
    slash = 47, /* / */

    _0 = 48,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,

    semicolon = 59, /* ; */
    equal = 61, /* = */

    a = 65,
    b,
    c,
    d,
    e,
    f,
    g,
    h,
    i,
    j,
    k,
    l,
    m,
    n,
    o,
    p,
    q,
    r,
    s,
    t,
    u,
    v,
    w,
    x,
    y,
    z,

    left_bracket = 91, /* [ */
    backslash = 92, /* \ */
    right_bracket = 93, /* ] */
    grave_accent = 96, /* ` */

    world_1 = 161, /* non-US #1 */
    world_2 = 162, /* non-US #2 */

    /* function keys */

    escape = 256,
    enter,
    tab,
    backspace,
    insert,
    del, /* delete */

    right,
    left,
    down,
    up,

    page_up,
    page_down,
    home,
    end,

    caps_lock = 280,
    scroll_lock,
    num_lock,
    print_screen,
    pause,

    f1 = 290,
    f2,
    f3,
    f4,
    f5,
    f6,
    f7,
    f8,
    f9,
    f10,
    f11,
    f12,
    f13,
    f14,
    f15,
    f16,
    f17,
    f18,
    f19,
    f20,
    f21,
    f22,
    f23,
    f24,
    f25,

    kp_0 = 320,
    kp_1,
    kp_2,
    kp_3,
    kp_4,
    kp_5,
    kp_6,
    kp_7,
    kp_8,
    kp_9,

    kp_decimal,
    kp_divide,
    kp_multiply,
    kp_subtract,
    kp_add,
    kp_enter,
    kp_equal,

    left_shift = 340,
    left_control,
    left_alt,
    left_super,

    right_shift,
    right_control,
    right_alt,
    right_super,

    menu,

    last = menu,
};

/**
 * @brief Input actions
 */
enum class action : type {
    release = 0,
    press,
    repeat
};

/**
 * @brief Input mods
 */
enum class mod : c8 {
    shift = 0x01,
    control = 0x02,
    alt = 0x04,
    super = 0x08,
    caps_lock = 0x10,
    num_lock = 0x20,
};

/**
 * @brief Key event
 */
struct key_event {
    /// Reference to key event
    using ref = key_event const&;

    /// Key event function
    using func = std::function<bool(ref)>;

    /// List of key event listeners
    using listeners = std::map<id, func>;

    /// List of key events
    using list = std::vector<key_event>;

    /// Sender id
    id sender;

    /// Input key
    lava::key key;

    /// Input action
    lava::action action;

    /// Input mod
    lava::mod mod;

    /// Input scan code
    i32 scancode = 0;

    /**
     * @brief Check if key is pressed
     * 
     * @param k         Key to check
     * 
     * @return true     Key is pressed
     * @return false    Key is not pressed
     */
    bool pressed(lava::key k) const {
        return action == action::press && key == k;
    }

    /**
     * @brief Check if key is released
     * 
     * @param k         Key to check
     * 
     * @return true     Key is released
     * @return false    Key is not released
     */
    bool released(lava::key k) const {
        return action == action::release && key == k;
    }

    /**
     * @brief Check if key is repeated
     * 
     * @param k         Key to check
     * 
     * @return true     Key is repeated
     * @return false    Key is not repeated
     */
    bool repeated(lava::key k) const {
        return action == action::repeat && key == k;
    }

    /**
     * @brief Check if key is active
     * 
     * @return true     Key is pressed or repeated
     * @return false    Key is not active
     */
    bool active() const {
        return action == action::press || action == action::repeat;
    }

    /**
     * @brief Check if key is pressed with mod
     * 
     * @param k         Key to check
     * @param m         Mod to check
     * 
     * @return true     Key is pressed with mod
     * @return false    Key is not pressed with mod
     */
    bool pressed(lava::key k, lava::mod m) const {
        return pressed(k) && mod == m;
    }
};

/**
 * @brief Input scroll offset
 */
struct scroll_offset {
    /// X offset
    r64 x = 0.0;

    /// Y offset
    r64 y = 0.0;
};

/**
 * @brief Scroll event
 */
struct scroll_event {
    /// Reference to scroll event
    using ref = scroll_event const&;

    /// Scroll event function
    using func = std::function<bool(ref)>;

    /// List of scroll event listeners
    using listeners = std::map<id, func>;

    /// List of scroll events
    using list = std::vector<scroll_event>;

    /// Sender id
    id sender;

    /// Input scroll offset
    scroll_offset offset;
};

/**
 * @brief Input mouse position
 */
struct mouse_position {
    /// X position
    r64 x = 0.0;

    /// Y position
    r64 y = 0.0;
};

/**
 * @brief Mouse move event
 */
struct mouse_move_event {
    /// Reference to mouse move event
    using ref = mouse_move_event const&;

    /// Mouse move event function
    using func = std::function<bool(ref)>;

    /// List of mouse move event listeners
    using listeners = std::map<id, func>;

    /// List of mouse move events
    using list = std::vector<mouse_move_event>;

    /// Sender id
    id sender;

    /// Input mouse position
    mouse_position position;
};

/**
 * @brief Input mouse buttons
 */
enum class mouse_button : type {
    _1 = 0,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,

    last = _8,

    left = _1,
    right = _2,
    middle = _3,
};

/**
 * @brief Mouse button event
 */
struct mouse_button_event {
    /// Reference to mouse button event
    using ref = mouse_button_event const&;

    /// Mouse button event function
    using func = std::function<bool(ref)>;

    /// List of mouse button event listeners
    using listeners = std::map<id, func>;

    /// List of mouse button events
    using list = std::vector<mouse_button_event>;

    /// Sender id
    id sender;

    /// Input mouse button
    mouse_button button;

    /// Input action
    lava::action action;

    /// Input mod
    lava::mod mod;

    /**
     * @brief Check if mouse button is pressed
     * 
     * @param b         Mouse button to check
     * 
     * @return true     Mouse button is pressed
     * @return false    Mouse button is not pressed
     */
    bool pressed(mouse_button b) const {
        return action == action::press && button == b;
    }

    /**
     * @brief Check if mouse button is released
     * 
     * @param b         Mouse button to check
     * 
     * @return true     Mouse button is released
     * @return false    Mouse button is not released
     */
    bool released(mouse_button b) const {
        return action == action::release && button == b;
    }
};

/**
 * @brief Path drop event
 */
struct path_drop_event {
    /// Reference to path drop event
    using ref = path_drop_event const&;

    /// Path drop event function
    using func = std::function<bool(ref)>;

    /// List of path drop event listeners
    using listeners = std::map<id, func>;

    /// List of path drop events
    using list = std::vector<path_drop_event>;

    /// Sender id
    id sender;

    /// List of files
    string_list files;
};

/**
 * @brief Mouse active event
 */
struct mouse_active_event {
    /// Reference to mouse active event
    using ref = mouse_active_event const&;

    /// Mouse active event function
    using func = std::function<bool(ref)>;

    /// List of mouse active event listeners
    using listeners = std::map<id, func>;

    /// List of mouse active events
    using list = std::vector<mouse_active_event>;

    /// Sender id
    id sender;

    /// Active state
    bool active = false;
};

/**
 * @brief Input callback
 */
struct input_callback {
    /// List of input callbacks
    using list = std::vector<input_callback*>;

    /**
     * @brief Input callback functions
     * 
     * @tparam T    Type of callback
     */
    template<typename T>
    using func = std::function<bool(typename T::ref)>;

    /// Called on key event
    key_event::func on_key_event;

    /// Called on scroll event
    scroll_event::func on_scroll_event;

    /// Called on mouse move event
    mouse_move_event::func on_mouse_move_event;

    /// Called on mouse button event
    mouse_button_event::func on_mouse_button_event;

    /// Called on mouse active event
    mouse_active_event::func on_mouse_active_event;

    /// Called on path drop event
    path_drop_event::func on_path_drop_event;
};

/**
 * @brief List of input events
 * 
 * @tparam T    Type of event
 */
template<typename T>
struct input_events : T::list {
    /**
     * @brief Add event to list
     * 
     * @param event    Event to add
     */
    void add(typename T::ref event) {
        this->push_back(event);
    }

    /// List of event listeners
    id_listeners<T> listeners;
};

/// List of key events
using input_key_events = input_events<key_event>;

/// List of scroll events
using input_scroll_events = input_events<scroll_event>;

/// List of mouse move events
using input_mouse_move_events = input_events<mouse_move_event>;

/// List of mouse button events
using input_mouse_button_events = input_events<mouse_button_event>;

/// List of mouse active events
using input_mouse_active_events = input_events<mouse_active_event>;

/// List of path drop events
using input_path_drop_events = input_events<path_drop_event>;

/// Input ignore result
constexpr bool const input_ignore = false;

/// Input done result
constexpr bool const input_done = true;

/**
 * @brief Input handling
 */
struct input : id_obj {
    /// List of key events
    input_key_events key;

    /// List of scroll events
    input_scroll_events scroll;

    /// List of mouse move events
    input_mouse_move_events mouse_move;

    /// List of mouse button events
    input_mouse_button_events mouse_button;

    /// List of mouse active events
    input_mouse_active_events mouse_active;

    /// List of path drop events
    input_path_drop_events path_drop;

    /**
     * @brief Handle events
     */
    void handle_events();

    /**
     * @brief Add callback to the input handling
     * 
     * @param callback    Callback to add
     */
    void add(input_callback* callback) {
        callbacks.push_back(callback);
    }

    /**
     * @brief Remove callback from the input handling
     * 
     * @param callback    Callback to remove
     */
    void remove(input_callback* callback) {
        lava::remove(callbacks, callback);
    }

    /**
     * @brief Get the mouse position
     * 
     * @return mouse_position    Current mouse position
     */
    mouse_position get_mouse_position() const {
        return current_position;
    }

    /**
     * @brief Set the mouse position
     * 
     * @param position    Current mouse position
     */
    void set_mouse_position(mouse_position const& position) {
        current_position = position;
    }

private:
    /**
     * @brief Handle mouse events
     */
    void handle_mouse_events();

    /// Current mouse position
    mouse_position current_position;

    /// List of input callbacks
    input_callback::list callbacks;
};

} // namespace lava
