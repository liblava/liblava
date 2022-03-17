/**
 * @file         liblava/util/random.hpp
 * @brief        Random generator
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/core/types.hpp>
#include <random>

namespace lava {

/**
 * @brief Random generator
 */
struct random_generator {
    /**
     * @brief Get singleton
     *
     * @return random_generator&    Random generator
     */
    static random_generator& instance() {
        static random_generator generator;
        return generator;
    }

    /**
     * @brief Get next random number
     *
     * @param low     Lowest number
     * @param high    Highest number
     *
     * @return i32    Random number
     */
    i32 get(i32 low, i32 high) {
        std::uniform_int_distribution<i32> dist(low, high);
        return dist(mt);
    }

    /**
     * @brief Get next real random number
     *
     * @tparam T      Type of number
     *
     * @param low     Lowest number
     * @param high    Highest number
     *
     * @return T      Random number
     */
    template<typename T = real>
    T get(T low, T high) {
        std::uniform_real_distribution<T> dist(low, high);
        return dist(mt);
    }

private:
    /**
     * @brief Construct a new random generator
     */
    random_generator() {
        std::random_device rd;
        mt = std::mt19937(rd());
    }

    /// Mersenne twister algorithm
    std::mt19937 mt;
};

/**
 * @brief Get next random number
 *
 * @tparam T      Type of number
 *
 * @param low     Lowest number
 * @param high    Highest number
 *
 * @return T      Random number
 */
template<typename T>
inline T random(T low, T high) {
    return random_generator::instance().get(low, high);
}

/**
 * @brief Get next random number (lowest is 0)
 *
 * @tparam T      Type of number
 *
 * @param high    Highest number
 *
 * @return T      Random number
 */
template<typename T>
inline T random(T high) {
    return random_generator::instance().get({}, high);
}

/**
 * @brief Pseudo random generator
 */
struct pseudo_random_generator {
    /**
     * @brief Construct a new pseudo random generator
     *
     * @param seed    Seed for generator
     */
    explicit pseudo_random_generator(ui32 seed)
    : seed(seed) {}

    /**
     * @brief Set the seed
     *
     * @param value    Generator seed
     */
    void set_seed(ui32 value) {
        seed = value;
    }

    /**
     * @brief Get next pseudo random number
     *
     * @return ui32    Random number
     */
    ui32 get() {
        return generate_fast() ^ (generate_fast() >> 7);
    }

private:
    /// Generator seed
    ui32 seed = 0;

    /**
     * @brief Generate fast random number
     *
     * @return ui32    Random number
     */
    ui32 generate_fast() {
        return seed = (seed * 196314165 + 907633515);
    }
};

} // namespace lava
