/**
 * @file         liblava/core/data.hpp
 * @brief        Data wrapper
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <string.h>
#include <liblava/core/types.hpp>

namespace lava {

/// Data pointer
using data_ptr = char*;

/// Const data pointer
using data_cptr = char const*;

/**
 * @brief Data provider
 */
struct data_provider {
    /**
     * @brief Allocation function
     */
    using alloc_func = std::function<data_ptr(size_t, size_t)>;

    /// Called on allocation
    alloc_func on_alloc;

    /**
     * @brief Free function
     */
    using free_func = std::function<void()>;

    /// Called on free
    free_func on_free;

    /**
     * @brief Reallocation function
     */
    using realloc_func = std::function<data_ptr(data_ptr, size_t, size_t)>;

    /// Called on reallocation
    realloc_func on_realloc;
};

/**
 * @brief Cast to data pointer
 *
 * @tparam T           Type to cast
 *
 * @param value        Value to cast
 *
 * @return data_ptr    Data pointer
 */
template<typename T>
inline data_ptr as_ptr(T* value) {
    return (data_ptr) value;
}

/**
 * @brief Align value up
 *
 * @tparam T       Type of value
 *
 * @param value    Value to align
 * @param align    Target alignment
 *
 * @return T       Aligned value
 */
template<typename T>
inline T align_up(T value, T align) {
    return (value + align - T(1)) / align * align;
}

/**
 * @brief Align a size
 *
 * @param size       Site to align
 * @param min        Minimal alignment
 *
 * @return size_t    Aligned size
 */
inline size_t align(size_t size, size_t min = 0) {
    if (min == 0)
        return align_up(size, sizeof(void*));

    return align_up((size + min - 1) & ~(min - 1), sizeof(void*));
}

/**
 * @brief Get alignment of type
 *
 * @tparam T         Type to align
 *
 * @param min        Minimal alignment
 *
 * @return size_t    Aligned size
 */
template<typename T>
inline size_t align(size_t min = 0) {
    return align(sizeof(T), min);
}

/**
 * @brief Allocate data
 *
 * @param size         Size of data
 * @param alignment    Target alignment
 *
 * @return void*       Allocated data
 */
inline void* alloc_data(size_t size, size_t alignment = sizeof(c8)) {
#if _WIN32
    return _aligned_malloc(size, alignment);
#else
    return aligned_alloc(alignment, size);
#endif
}

/**
 * @brief Free data
 *
 * @param data    Data to free
 */
inline void free_data(void* data) {
#if _WIN32
    _aligned_free(data);
#else
    free(data);
#endif
}

/**
 * @brief Reallocate data
 *
 * @param data         Data to reallocate
 * @param size         Size of data
 * @param alignment    Target alignment
 *
 * @return void*       Reallocated data
 */
inline void* realloc_data(void* data, size_t size, size_t alignment) {
#if _WIN32
    return _aligned_realloc(data, size, alignment);
#else
    return realloc(data, align(size, alignment));
#endif
}

/**
 * @brief Data modes
 */
enum class data_mode : type {
    alloc = 0,
    no_alloc
};

/**
 * @brief Data wrapper
 */
struct data {
    /**
     * @brief Construct a new data
     */
    data() = default;

    /**
     * @brief Construct a new data
     *
     * @param ptr     Data pointer
     * @param size    Size of data
     */
    data(void* ptr, size_t size)
    : ptr(as_ptr(ptr)), size(size) {}

    /**
     * @brief Set and allocate data by length
     *
     * @param length   Length of data
     * @param mode     Data mode
     */
    void set(size_t length, data_mode mode = data_mode::alloc) {
        size = length;
        alignment = align<data_ptr>();

        if (mode == data_mode::alloc)
            allocate();
    }

    /**
     * @brief Allocate data
     *
     * @return true     Allocate was successful
     * @return false    Allocate failed
     */
    bool allocate() {
        ptr = as_ptr(alloc_data(size, alignment));
        return ptr != nullptr;
    }

    /**
     * @brief Free data
     */
    void free() {
        if (!ptr)
            return;

        free_data(ptr);
        ptr = nullptr;
    }

    /// Pointer to data
    data_ptr ptr = nullptr;

    /// Size of data
    size_t size = 0;

    /// Data alignment
    size_t alignment = 0;
};

/**
 * @brief Const data wrapper
 */
struct cdata {
    /**
     * @brief Construct a new const data
     */
    cdata() = default;

    /**
     * @brief Construct a new const data
     *
     * @param ptr       Pointer to data
     * @param length    Length of data
     */
    cdata(const void* ptr, size_t length)
    : ptr(as_ptr(ptr)), size(length) {}

    /**
     * @brief Construct a new const data from other data
     *
     * @param data    Source data
     */
    cdata(data const& data)
    : cdata(data.ptr, data.size) {}

    /// Const data pointer
    data_cptr ptr = nullptr;

    /// Size of data
    size_t size = 0;
};

/**
 * @brief Unique data wrapper
 */
struct unique_data : data {
    /**
     * @brief Construct a new unique data
     *
     * @param length    Length of data
     * @param mode      Data mode
     */
    unique_data(size_t length = 0, data_mode mode = data_mode::alloc) {
        if (length)
            set(length, mode);
    }

    /**
     * @brief Construct a new unique data from another data
     *
     * @param data    Source data
     */
    explicit unique_data(data const& data) {
        ptr = data.ptr;
        size = data.size;
        alignment = data.alignment;
    }

    /**
     * @brief Destroy the unique data
     */
    ~unique_data() {
        free();
    }
};

/**
 * @brief Get next power of two
 *
 * @param x          Source value
 *
 * @return size_t    Next power of two
 */
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

#ifndef __GNUC__
    /// Duplicate string
    #define strndup(p, n) _strdup(p)
#endif

/**
 * @brief Convert data size to human readable string
 *
 * @param sz        Size to convert
 *
 * @return char*    Human readable size string
 */
inline char* human_readable(size_t const sz) {
    static auto const buffer_size = 32;

    char const prefixes[] = "KMGTPEZY";
    char buf[buffer_size];
    auto which = -1;

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

} // namespace lava
