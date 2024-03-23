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
      id(msg), dispatch_time(dispatch_time),
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
               && (id == rhs.id);
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
    index id = no_index;

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
        pool.setup(thread_count);
    }

    /**
     * @brief Tear down the dispatcher
     */
    void teardown() {
        pool.teardown();
    }

    /**
     * @brief Update the dispatcher
     * @param current    Time in milliseconds
     */
    void update(ms current) {
        current_time = current;
        dispatch_delayed_messages(current_time);
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
                     current_time,
                     info);

        if (delay == ms{0}) {
            discharge(msg); // now
            return;
        }

        msg.dispatch_time += delay;
        messages.insert(msg);
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
        std::lock_guard guard(lock);

        if (dispatches.count(target))
            return false;

        dispatches.emplace(target, func);
        return true;
    }

    /**
     * @brief Remove dispatch
     * @param target    Sender id
     * @return Dispatch removed or not
     */
    bool remove_dispatch(id::ref target) {
        std::lock_guard guard(lock);

        if (!dispatches.count(target))
            return false;

        dispatches.erase(target);
        return true;
    }

    /**
     * @brief Check if dispatch is registered
     * @param target    Sender id
     * @return Dispatch exists or not
     */
    bool has_dispatch(id::ref target) const {
        return dispatches.count(target);
    }

private:
    /**
     * @brief Discharge a message
     * @param message    Message to discharge
     */
    void discharge(telegram::ref message) {
        pool.enqueue([&, message](id::ref thread_id) {
            std::lock_guard guard(lock);

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
        while (!messages.empty()
               && (messages.begin()->dispatch_time < time)) {
            discharge(*messages.begin());

            messages.erase(messages.begin());
        }
    }

    /**
     * @brief Get dispatch receiver
     * @param target    Receiver id
     */
    message_func get_dispatch(id::ref target) {
        if (!dispatches.count(target))
            return nullptr;

        return dispatches.at(target);
    }

    /// Map of dispatches
    using dispatch_map = std::map<id, message_func>;

    /// Registered dispatches
    dispatch_map dispatches;

    /// Lock for dispatches
    std::mutex lock;

    /// Time in milliseconds
    ms current_time;

    /// Thread pool
    thread_pool pool;

    /// List of messages
    telegram::set messages;
};

} // namespace lava
