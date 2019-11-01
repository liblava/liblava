// file      : liblava/util/telegram.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/util/thread.hpp>

#include <any>
#include <cmath>
#include <set>

namespace lava {

constexpr milliseconds const telegram_min_delay{ 250 };

struct telegram {

    using ref = telegram const&;

    explicit telegram(id::ref sender, id::ref receiver, type msg, milliseconds dispatch_time = {}, std::any info = {})
                        : sender(sender), receiver(receiver), msg(msg), dispatch_time(dispatch_time), info(std::move(info)) {}

    bool operator==(ref rhs) const {

        return ((dispatch_time - rhs.dispatch_time) < telegram_min_delay) &&
                    (sender == rhs.sender) && (receiver == rhs.receiver) && (msg == rhs.msg);
    }

    bool operator<(ref rhs) const {

        if (*this == rhs)
            return false;

        return (dispatch_time < rhs.dispatch_time);
    }

    id sender;
    id receiver;

    type msg = no_type;

    milliseconds dispatch_time;
    std::any info;
};

struct dispatcher {

    void setup(ui32 threadcount) { pool.setup(threadcount); }

    void teardown() { pool.teardown(); }

    void update(milliseconds current) {

        current_time = current;
        dispatch_delayed_messages(current_time);
    }

    void add_message(id::ref receiver, id::ref sender, type message, milliseconds delay = {}, std::any const& info = {}) {

        telegram msg(sender, receiver, message, current_time, info);

        if (delay == milliseconds{}) {

            discharge(msg); // now
            return;
        }

        msg.dispatch_time += delay;
        messages.insert(msg);
    }

    using message_func = std::function<void(telegram::ref, id::ref)>;
    message_func on_message;

private:
    void discharge(telegram::ref message) {

        pool.enqueue([&, message](id::ref thread) {
            if (on_message)
                on_message(message, thread);
        });
    }

    void dispatch_delayed_messages(milliseconds time) {

        while (!messages.empty() && (messages.begin()->dispatch_time < time) && (messages.begin()->dispatch_time > milliseconds{})) {

            discharge(*messages.begin());
            messages.erase(messages.begin());
        }
    }

    milliseconds current_time;

    thread_pool pool;
    std::set<telegram> messages;
};

} // lava
