// file      : liblava/core/data.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/types.hpp>

#include <string.h>

namespace lava {

using data_ptr = char*;
using data_cptr = char const*;

struct data_provider {

    using alloc_func = std::function<data_ptr(size_t, size_t)>;
    alloc_func on_alloc;

    using free_func = std::function<void()>;
    free_func on_free;

    using realloc_func = std::function<data_ptr(data_ptr, size_t, size_t)>;
    realloc_func on_realloc;
};

template <typename T>
inline data_ptr as_ptr(T* value) { return (data_ptr)value; }

struct data {

    data() = default;
    data(void* ptr, size_t size) : ptr(as_ptr(ptr)), size(size) {}

    data_ptr ptr = nullptr;

    size_t size = 0;
    size_t alignment = 0;
};

template <typename T>
inline T align_up(T val, T align) { return (val + align - 1) / align * align; }

inline size_t align(size_t size, size_t min = 0) {

    if (min == 0)
        return align_up(size, sizeof(void*));

    return align_up((size + min - 1) & ~(min - 1), sizeof(void*));
}

template <typename T>
inline size_t align(size_t min = 0) { return align(sizeof(T), min); }

inline void* alloc_data(size_t size, size_t alignment = sizeof(c8)) {
#if _WIN32
    return _aligned_malloc(size, alignment);
#else
    return aligned_alloc(alignment, size);
#endif
}

inline void free_data(void* data) {
#if _WIN32
    _aligned_free(data);
#else
    free(data);
#endif
}

inline void* realloc_data(void* data, size_t size, size_t alignment) {
#if _WIN32
    return _aligned_realloc(data, size, alignment);
#else
    return realloc(data, align(size, alignment));
#endif
}

inline size_t next_pow_2(size_t x) {

    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}

struct scope_data : data {

    explicit scope_data(size_t length = 0, bool alloc = true) {

        if (length)
            set(length, alloc);
    }

    explicit scope_data(i64 length, bool alloc = true) : scope_data(to_size_t(length), alloc) {}
    explicit scope_data(data const& data) {

        ptr = data.ptr;
        size = data.size;
        alignment = data.alignment;
    }

    ~scope_data() {

        free();
    }

    void set(size_t length, bool alloc = true) {

        size = length;
        alignment = align<data_ptr>();

        if (alloc)
            allocate();
    }

    bool allocate() {

        ptr = as_ptr(alloc_data(size, alignment));
        return ptr != nullptr;
    }

    void free() {

        if (!ptr)
            return;

        free_data(ptr);
        ptr = nullptr;
    }
};

#ifndef __GNUC__
#define strndup(p, n) _strdup(p)
#endif

inline char* human_readable(size_t const sz) {

    static ui32 const buffer_size = 32;

    char const prefixes[] = "KMGTPEZY";
    char buf[buffer_size];
    i32 which = -1;

    auto result = to_r64(sz);
    while (result > 1024 && which < 7) {

        result /= 1024;
        ++which;
    }

    char unit[] = "\0i";
    if (which >= 0)
        unit[0] = prefixes[which];

    snprintf(buf, buffer_size, "%.2f %sB", result, unit);
    return strndup(buf, buffer_size);
}

#ifndef __GNUC__
#undef strndup
#endif

} // lava
