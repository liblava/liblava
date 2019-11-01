// file      : liblava/core/time.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/types.hpp>

#include <chrono>
#include <iomanip>
#include <sstream>

namespace lava {

using seconds = std::chrono::seconds;
using milliseconds = std::chrono::milliseconds;

using clock = std::chrono::high_resolution_clock;
using time_point = clock::time_point;
using duration = clock::duration;

inline r64 to_sec(milliseconds ms) { return ms.count() / 1000.0; }
inline milliseconds to_ms(r64 sec) { return milliseconds(to_i32(sec * 1000.0)); }

struct timer {

    timer() : start_time(clock::now()) {}

    void reset() { start_time = clock::now(); }

    milliseconds elapsed() const {

        return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start_time);
    }

private:
    time_point start_time;
};

struct run_time {

    milliseconds current;
    milliseconds clock{ 16 };

    milliseconds system;
    milliseconds delta;

    bool use_fix_delta = false;
    milliseconds fix_delta{ 20 };

    r32 speed = 1.f;
    bool paused = false;
};

#pragma warning(push)
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

template <typename CLOCK_TYPE = std::chrono::system_clock>
inline string time_stamp(const typename CLOCK_TYPE::time_point& time_point, string_ref format = "%Y-%m-%d %H-%M-%S") {

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch()) % 1000;

    const std::time_t t = CLOCK_TYPE::to_time_t(time_point);
    const std::tm tm = *std::localtime(std::addressof(t));

    std::ostringstream stm;
    stm << std::put_time(std::addressof(tm), format.c_str()) << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return stm.str();
}

inline string get_current_time_and_date() {

    auto now = std::chrono::system_clock::now();
    return time_stamp(now);
}

#pragma warning(pop)

} // lava
