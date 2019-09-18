// file      : liblava/core/time.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/types.hpp>

#include <chrono>
#include <iomanip>
#include <sstream>

namespace lava {

inline time ms_in_seconds(ui32 ms) { return ms / 1000.f; }

inline ui32 seconds_in_ms(time sec) { return to_ui32(sec * 1000.f); }

struct time_info {

    ui32 hours = 0;
    ui32 minutes = 0;
    ui32 seconds = 0;
    ui32 ms = 0;
};

inline time to_time(ui32 hours = 0, ui32 minutes = 0, ui32 seconds = 0, ui32 ms = 0) {

    return hours * 3600 + minutes * 60 + seconds + ms / 1000.;
}

inline time to_time(time_info info) {

    return to_time(info.hours, info.minutes, info.seconds, info.ms);
}

inline time_info to_time(time time) {

    time_info info;
    info.hours = to_ui32(time / 3600);
    auto temp = time - info.hours * 3600;
    info.minutes = to_ui32(temp / 60);
    temp = temp - info.minutes * 60;
    info.seconds = to_ui32(temp);
    temp = temp - info.seconds;
    info.ms = to_ui32(temp * 1000);
    return info;
}

using time_point = std::chrono::high_resolution_clock::time_point;
using duration = std::chrono::high_resolution_clock::duration;

inline float to_float_seconds(duration d) {

    return std::chrono::duration_cast<std::chrono::duration<float>>(d).count();
}

struct timer {

    timer() : time_point(clock::now()) {}

    void reset() { time_point = clock::now(); }

    time elapsed() const {

        return std::chrono::duration_cast<second>(clock::now() - time_point).count();
    }

private:
    using clock = std::chrono::high_resolution_clock;
    using second = std::chrono::duration<time, std::ratio<1>>;

    std::chrono::time_point<clock> time_point;
};

struct run_time {

    time seconds = 0.;
    time clock = 0.016;

    time system = 0.;
    time delta = 0.;

    bool use_fix_delta = false;
    time fix_delta = 0.02;

    r32 speed = 1.f;
    bool paused = false;

    time get_speed_delta() const { return delta * speed; }
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
