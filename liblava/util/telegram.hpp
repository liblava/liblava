/**
 * @file         liblava/util/telegram.hpp
 * @brief        Telegram dispatcher
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <any>
#include <cmath>
#include <liblava/util/thread.hpp>
#include <set>

namespace lava {

/// Minimal telegram delay in milliseconds
constexpr ms const telegram_min_delay{ 250 };

/// Any type
using any = std::any;

/**
 * @brief Telegram
 */
struct telegram {
    /// Reference to telegram
    using ref = telegram const&;

    /**
     * @brief Construct a new telegram
     *
     * @param sender           Sender id
     * @param receiver         Receiver id
     * @param msg              Telegram message
     * @param dispatch_time    Dispatch time
     * @param info             Telegram information
     */
    explicit telegram(id::ref sender,
                      id::ref receiver,
                      type msg,
                      ms dispatch_time = {},
                      any info = {})
    : sender(sender), receiver(receiver),
      msg(msg), dispatch_time(dispatch_time),
      info(std::move(info)) {}

    /**
     * @brief Equal operator
     *
     * @param rhs       Another telegram
     *
     * @return true     Telegram is equal
     * @return false    Telegram is inequal
     */
    bool operator==(ref rhs) const {
        return ((dispatch_time - rhs.dispatch_time) < telegram_min_delay)
               && (sender == rhs.sender)
               && (receiver == rhs.receiver)
               && (msg == rhs.msg);
    }

    /**
     * @brief Time order operator
     *
     * @param rhs       Another telegram
     *
     * @return true     Telegram is earlier
     * @return false    Telegram is later
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

    /// Telegram message
    type msg = no_type;

    /// Dispatch time
    ms dispatch_time;

    /// Telegram information
    any info;
};

/**
 * @brief Telegram dispatcher
 */
struct dispatcher {
    /**
     * @brief Set up the dispatcher
     *
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
     *
     * @param current    Time in milliseconds
     */
    void update(ms current) {
        current_time = current;
        dispatch_delayed_messages(current_time);
    }

    /**
     * @brief Add message to dispatcher
     *
     * @param receiver    Receiver id
     * @param sender      Sender id
     * @param message     Telegram message
     * @param delay       Delay time
     * @param info        Telegram information
     */
    void add_message(id::ref receiver,
                     id::ref sender,
                     type message,
                     ms delay = {},
                     any const& info = {}) {
        telegram msg(sender,
                     receiver,
                     message,
                     current_time,
                     info);

        if (delay == ms{ 0 }) {
            discharge(msg); // now
            return;
        }

        msg.dispatch_time += delay;
        messages.insert(msg);
    }

    /// Message function
    using message_func = std::function<void(telegram::ref, id::ref)>;

    /// Called on message handling
    message_func on_message;

private:
    /**
     * @brief Discharge a message
     *
     * @param message    Message to discharge
     */
    void discharge(telegram::ref message) {
        pool.enqueue([&, message](id::ref thread) {
            if (on_message)
                on_message(message, thread);
        });
    }

    /**
     * @brief Dispatch delayed messages
     *
     * @param time    Current time
     */
    void dispatch_delayed_messages(ms time) {
        while (!messages.empty()
               && (messages.begin()->dispatch_time < time)
               && (messages.begin()->dispatch_time > ms{})) {
            discharge(*messages.begin());

            messages.erase(messages.begin());
        }
    }

    /// Time in milliseconds
    ms current_time;

    /// Thread pool
    thread_pool pool;

    /// List of messages
    std::set<telegram> messages;
};

} // namespace lava
