/**
 * @file         liblava/core/time.hpp
 * @brief        Run time
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/core/types.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace lava {

/// Seconds
using seconds = std::chrono::seconds;

/// Milliseconds
using milliseconds = std::chrono::milliseconds;

/// Milliseconds
using ms = milliseconds;

/// Microseconds
using microseconds = std::chrono::microseconds;

/// Microseconds
using us = microseconds;

/// One second
constexpr seconds const one_second = seconds(1);

/// One millisecond
constexpr ms const one_ms = ms(1);

/// One microsecond
constexpr us const one_us = us(1);

/// Clock
using clock = std::chrono::high_resolution_clock;

/// Time point
using time_point = clock::time_point;

/// Duration
using duration = clock::duration;

/**
 * @brief Convert milliseconds to delta
 * @param ms        Milliseconds to convert
 * @return delta    Converted delta
 */
inline delta to_delta(milliseconds ms) {
    return ms.count() / 1000.f;
}

/**
 * @see to_delta()
 */
inline delta to_dt(milliseconds ms) {
    return to_delta(ms);
}

/**
 * @brief Convert milliseconds to seconds
 * @param ms       Milliseconds to convert
 * @return real    Converted seconds
 */
inline real to_sec(milliseconds ms) {
    return ms.count() / 1000.;
}

/**
 * @brief Convert milliseconds to fixed seconds
 * @param ms      Milliseconds to convert
 * @return i32    Converted fixed seconds
 */
inline i32 to_sec_fix(milliseconds ms) {
    return to_i32(to_sec(ms));
}

/**
 * @brief Convert delta to milliseconds
 * @param dt     Delta to convert
 * @return ms    Converted milliseconds
 */
inline ms to_ms(delta dt) {
    return ms(to_i32(dt * 1000.f));
}

/**
 * @brief Convert seconds to milliseconds
 * @param sec    Seconds to convert
 * @return ms    Converted milliseconds
 */
inline ms to_ms(real sec) {
    return ms(to_i32(sec * 1000.));
}

/**
 * @brief Timer
 */
struct timer {
    /**
     * @brief Construct a new timer
     */
    timer()
    : m_start_time(clock::now()) {}

    /**
     * @brief Reset the timer
     */
    void reset() {
        m_start_time = clock::now();
    }

    /**
     * @brief Get the elapsed time
     * @return ms    Elapsed milliseconds
     */
    ms elapsed() const {
        return std::chrono::duration_cast<ms>(clock::now()
                                              - m_start_time);
    }

private:
    /// Start time
    time_point m_start_time;
};

/**
 * @brief Run time
 */
struct run_time {
    /// Current milliseconds
    ms current{0};

    /// Clock milliseconds
    ms clock{16};

    /// System milliseconds
    ms system{0};

    /// Delta milliseconds
    ms delta{0};

    /// Fix delta milliseconds (0 = deactivated)
    ms fix_delta{0};

    /// Speed factor
    r32 speed = 1.f;

    /// Paused run time
    bool paused = false;
};

#ifdef _WIN32
    #pragma warning(push)
    #pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
#endif

/**
 * @brief Convert timestamp to string
 * @tparam CLOCK_TYPE    Clock type
 * @param time_point     Time point
 * @param format         String format
 * @return string        Converted string
 */
template <typename CLOCK_TYPE = std::chrono::system_clock>
inline string timestamp(const typename CLOCK_TYPE::time_point& time_point,
                        string_ref format = "%Y-%m-%d %H-%M-%S") {
    auto ms = std::chrono::duration_cast<milliseconds>(
                  time_point.time_since_epoch())
              % 1000;

    auto const t = CLOCK_TYPE::to_time_t(time_point);
    auto const tm = *std::localtime(std::addressof(t));

    std::ostringstream stm;
    stm << std::put_time(std::addressof(tm), str(format))
        << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return stm.str();
}

/**
 * @brief Get the current time as string
 * @return string    Time and date representation
 */
inline string get_current_time() {
    auto now = std::chrono::system_clock::now();
    return timestamp(now);
}

#ifdef _WIN32
    #pragma warning(pop)
#endif

/**
 * @brief Get the current timestamp in milliseconds
 * @return ms    Timestamp in ms
 */
inline ms get_current_timestamp_ms() {
    return std::chrono::duration_cast<ms>(
        clock::now().time_since_epoch());
}

/**
 * @brief Get the current timestamp in microseconds
 * @return us    Timestamp in us
 */
inline us get_current_timestamp_us() {
    return std::chrono::duration_cast<us>(
        clock::now().time_since_epoch());
}

/**
 * @brief Get the current timestamp in milliseconds (uint)
 * @return ui64    Timestamp in ms (uint)
 */
inline ui64 get_current_timestamp() {
    return get_current_timestamp_ms().count();
}

} // namespace lava
