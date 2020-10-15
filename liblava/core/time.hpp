// file      : liblava/core/time.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <chrono>
#include <iomanip>
#include <liblava/core/types.hpp>
#include <sstream>

namespace lava {

    using seconds = std::chrono::seconds;
    using milliseconds = std::chrono::milliseconds;
    using ms = milliseconds;

    constexpr seconds const one_second = seconds(1);
    constexpr ms const one_ms = ms(1);

    using clock = std::chrono::high_resolution_clock;
    using time_point = clock::time_point;
    using duration = clock::duration;

    inline delta to_delta(milliseconds ms) {
        return ms.count() / 1000.f;
    }
    inline r64 to_sec(milliseconds ms) {
        return ms.count() / 1000.;
    }
    inline ms to_ms(delta dt) {
        return ms(to_i32(dt * 1000.f));
    }
    inline ms to_ms(r64 sec) {
        return ms(to_i32(sec * 1000.));
    }

    struct timer {
        timer()
        : start_time(clock::now()) {}

        void reset() {
            start_time = clock::now();
        }

        ms elapsed() const {
            return std::chrono::duration_cast<ms>(clock::now() - start_time);
        }

    private:
        time_point start_time;
    };

    struct run_time {
        ms current{ 0 };
        ms clock{ 16 };

        ms system{ 0 };
        ms delta{ 0 };

        bool use_fix_delta = false;
        ms fix_delta{ 20 };

        r32 speed = 1.f;
        bool paused = false;
    };

#pragma warning(push)
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

    template<typename CLOCK_TYPE = std::chrono::system_clock>
    inline string time_stamp(const typename CLOCK_TYPE::time_point& time_point, string_ref format = "%Y-%m-%d %H-%M-%S") {
        auto ms = std::chrono::duration_cast<milliseconds>(time_point.time_since_epoch()) % 1000;

        auto const t = CLOCK_TYPE::to_time_t(time_point);
        auto const tm = *std::localtime(std::addressof(t));

        std::ostringstream stm;
        stm << std::put_time(std::addressof(tm), str(format)) << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return stm.str();
    }

    inline string get_current_time_and_date() {
        auto now = std::chrono::system_clock::now();
        return time_stamp(now);
    }

#pragma warning(pop)

} // namespace lava
