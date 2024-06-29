/**
 * @file         liblava/util/telegram.hpp
 * @brief        Message dispatcher
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/util/thread.hpp"
#include <any>
#include <cmath>
#include <set>

namespace lava {

/// Minimal telegram delay in milliseconds
constexpr ms const telegram_min_delay{250};

/// Any type
using any = std::any;

/**
 * @brief Telegram
 */
struct telegram {
    /// Reference to telegram
    using ref = telegram const&;

    /// Set of telegrams
    using set = std::set<telegram>;

    /**
     * @brief Construct a new telegram
     * @param sender           Sender id
     * @param receiver         Receiver id
     * @param msg              Message id
     * @param dispatch_time    Dispatch time
     * @param info             Telegram information
     */
    explicit telegram(id::ref sender,
                      id::ref receiver,
                      index msg,
                      ms dispatch_time = {},
                      any info = {})
    : sender(sender), receiver(receiver),
      msg_id(msg), dispatch_time(dispatch_time),
      info(std::move(info)) {}

    /**
     * @brief Equal operator
     * @param rhs    Another telegram
     * @return Telegram is equal or not
     */
    bool operator==(ref rhs) const {
        return ((dispatch_time - rhs.dispatch_time) < telegram_min_delay)
               && (sender == rhs.sender)
               && (receiver == rhs.receiver)
               && (msg_id == rhs.msg_id);
    }

    /**
     * @brief Time order operator
     * @param rhs    Another telegram
     * @return Telegram is earlier or later
     */
    bool operator<(ref rhs) const {
        if (*this == rhs)
            return false;

        return (dispatch_time < rhs.dispatch_time);
    }

    /// Sender id
    id sender;

    /// Receiver id
    id receiver;

    /// Message id
    index msg_id = no_index;

    /// Dispatch time
    ms dispatch_time;

    /// Telegram information
    any info;
};

/**
 * @brief Telegraph station
 */
struct telegraph : interface {
    /**
     * @brief Send message to dispatcher
     * @param receiver    Receiver id
     * @param sender      Sender id
     * @param message     Message id
     * @param delay       Delay time
     * @param info        Telegram information
     */
    virtual void send_message(id::ref receiver,
                              id::ref sender,
                              index message,
                              ms delay = {},
                              any const& info = {}) = 0;
};

/**
 * @brief Message dispatcher
 */
struct message_dispatcher : telegraph {
    /**
     * @brief Destroy the dispatcher
     */
    ~message_dispatcher() {
        teardown();
    }

    /**
     * @brief Set up the dispatcher
     * @param thread_count    Number of threads
     */
    void setup(ui32 thread_count) {
        m_pool.setup(thread_count);
    }

    /**
     * @brief Tear down the dispatcher
     */
    void teardown() {
        m_pool.teardown();
    }

    /**
     * @brief Update the dispatcher
     * @param current    Time in milliseconds
     */
    void update(ms current) {
        m_current_time = current;
        dispatch_delayed_messages(m_current_time);
    }

    /// @see telegraph::send_message
    void send_message(id::ref receiver,
                      id::ref sender,
                      index message,
                      ms delay = {},
                      any const& info = {}) override {
        telegram msg(sender,
                     receiver,
                     message,
                     m_current_time,
                     info);

        if (delay == ms{0}) {
            discharge(msg); // now
            return;
        }

        msg.dispatch_time += delay;
        m_messages.insert(msg);
    }

    /// Message function
    using message_func = std::function<void(telegram::ref, id::ref)>;

    /**
     * @brief Add dispatch
     * @param target    Sender id
     * @param func      Dispatch function
     * @return Dispatch added or not
     */
    bool add_dispatch(id::ref target, message_func func) {
        std::lock_guard guard(m_lock);

        if (m_dispatches.count(target))
            return false;

        m_dispatches.emplace(target, func);
        return true;
    }

    /**
     * @brief Remove dispatch
     * @param target    Sender id
     * @return Dispatch removed or not
     */
    bool remove_dispatch(id::ref target) {
        std::lock_guard guard(m_lock);

        if (!m_dispatches.count(target))
            return false;

        m_dispatches.erase(target);
        return true;
    }

    /**
     * @brief Check if dispatch is registered
     * @param target    Sender id
     * @return Dispatch exists or not
     */
    bool has_dispatch(id::ref target) const {
        return m_dispatches.count(target);
    }

private:
    /**
     * @brief Discharge a message
     * @param message    Message to discharge
     */
    void discharge(telegram::ref message) {
        m_pool.enqueue([&, message](id::ref thread_id) {
            std::lock_guard guard(m_lock);

            auto dispatch = get_dispatch(message.receiver);
            LAVA_ASSERT(dispatch);
            if (dispatch)
                dispatch(message, thread_id);
        });
    }

    /**
     * @brief Dispatch delayed messages
     * @param time    Current time
     */
    void dispatch_delayed_messages(ms time) {
        while (!m_messages.empty()
               && (m_messages.begin()->dispatch_time < time)) {
            discharge(*m_messages.begin());

            m_messages.erase(m_messages.begin());
        }
    }

    /**
     * @brief Get dispatch receiver
     * @param target    Receiver id
     */
    message_func get_dispatch(id::ref target) {
        if (!m_dispatches.count(target))
            return nullptr;

        return m_dispatches.at(target);
    }

    /// Map of dispatches
    using dispatch_map = std::map<id, message_func>;

    /// Registered dispatches
    dispatch_map m_dispatches;

    /// Lock for dispatches
    std::mutex m_lock;

    /// Time in milliseconds
    ms m_current_time;

    /// Thread pool
    thread_pool m_pool;

    /// List of messages
    telegram::set m_messages;
};

} // namespace lava
