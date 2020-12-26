// file      : liblava/util/utility.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <algorithm>
#include <cstring>
#include <liblava/core/types.hpp>
#include <utility>

namespace lava {

    inline bool exists(names_ref list, name item) {
        auto itr = std::find_if(list.begin(), list.end(), [&](name entry) { return strcmp(entry, item) == 0; });
        return itr != list.end();
    }

    template<typename T>
    inline void remove(std::vector<T>& list, T item) {
        list.erase(std::remove(list.begin(), list.end(), item), list.end());
    }

    template<typename T>
    inline bool contains(std::vector<T>& list, T item) {
        return std::find(list.begin(), list.end(), item) != list.end();
    }

    template<typename T>
    inline void append(std::vector<T>& list, std::vector<T>& items) {
        list.insert(list.end(), items.begin(), items.end());
    }

    // reversed iterable

    template<typename T>
    struct reversion_wrapper { T& iterable; };

    template<typename T>
    inline auto begin(reversion_wrapper<T> w) {
        return std::rbegin(w.iterable);
    }

    template<typename T>
    inline auto end(reversion_wrapper<T> w) {
        return std::rend(w.iterable);
    }

    template<typename T>
    inline reversion_wrapper<T> reverse(T&& iterable) {
        return { iterable };
    }

} // namespace lava
