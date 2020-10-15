// file      : liblava/util/random.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/types.hpp>
#include <random>

namespace lava {

    struct random_generator {
        static random_generator& instance() {
            static random_generator generator;
            return generator;
        }

        i32 get(i32 low, i32 high) {
            std::uniform_int_distribution<i32> dist(low, high);
            return dist(mt);
        }

        template<typename T = real>
        T get(T low, T high) {
            std::uniform_real_distribution<T> dist(low, high);
            return dist(mt);
        }

    private:
        random_generator() {
            std::random_device rd;
            mt = std::mt19937(rd());
        }

        std::mt19937 mt;
    };

    template<typename T>
    inline T random(T low, T high) {
        return random_generator::instance().get(low, high);
    }

    template<typename T>
    inline T random(T high) {
        return random_generator::instance().get({}, high);
    }

    struct pseudo_random_generator {
        explicit pseudo_random_generator(ui32 seed)
        : seed(seed) {}

        void set_seed(ui32 value) {
            seed = value;
        }
        ui32 get() {
            return generate_fast() ^ (generate_fast() >> 7);
        }

    private:
        ui32 seed = 0;
        ui32 generate_fast() {
            return seed = (seed * 196314165 + 907633515);
        }
    };

} // namespace lava
