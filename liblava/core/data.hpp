/**
 * @file         liblava/core/data.hpp
 * @brief        Data wrapper
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <string.h>
#include "liblava/core/types.hpp"

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
 * @param value        Value to cast
 * @return data_ptr    Data pointer
 */
inline data_ptr as_ptr(auto* value) {
    return (data_ptr) value;
}

/**
 * @brief Cast to const data pointer
 * @param value         Value to cast
 * @return data_cptr    Const data pointer
 */
inline data_cptr as_cptr(auto* value) {
    return (data_cptr) value;
}

/**
 * @brief Align value up
 * @param value    Value to align
 * @param align    Target alignment
 * @return auto    Aligned value
 */
inline auto align_up(auto value,
                     auto align) {
    return (value + align - 1) / align * align;
}

/**
 * @brief Align a size
 * @param size       Site to align
 * @param min        Minimal alignment
 * @return size_t    Aligned size
 */
inline size_t align(size_t size,
                    size_t min = 0) {
    if (min == 0)
        return align_up(size, sizeof(void*));

    return align_up((size + min - 1) & ~(min - 1), sizeof(void*));
}

/**
 * @brief Get alignment of type
 * @tparam T         Type to align
 * @param min        Minimal alignment
 * @return size_t    Aligned size
 */
template<typename T>
inline size_t align(size_t min = 0) {
    return align(sizeof(T), min);
}

/**
 * @brief Allocate data
 * @param size         Size of data
 * @param alignment    Target alignment
 * @return void*       Allocated data
 */
inline void* alloc_data(size_t size,
                        size_t alignment = sizeof(c8)) {
#if _WIN32
    return _aligned_malloc(size, alignment);
#else
    if (size % alignment == 0) {
        return aligned_alloc(alignment, size);
    } else {
        return aligned_alloc(alignment, ((size / alignment) + 1) * alignment);
    }
#endif
}

/**
 * @brief Free data
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
 * @param data         Data to reallocate
 * @param size         Size of data
 * @param alignment    Target alignment
 * @return void*       Reallocated data
 */
inline void* realloc_data(void* data,
                          size_t size,
                          size_t alignment) {
#if _WIN32
    return _aligned_realloc(data, size, alignment);
#else
    return realloc(data, align(size, alignment));
#endif
}

/**
 * @brief Data modes
 */
enum class data_mode : index {
    alloc = 0,
    no_alloc
};

/**
 * @brief Data wrapper
 */
struct data {
    /// Reference to data wrapper
    using ref = data const&;

    /**
     * @brief Construct a new data
     */
    data() = default;

    /**
     * @brief Construct a new data
     * @param ptr     Data pointer
     * @param size    Size of data
     */
    data(auto* ptr, size_t size)
    : ptr(as_ptr(ptr)), size(size) {}

    /**
     * @brief Set and allocate data by length
     * @param length   Length of data
     * @param mode     Data mode
     * @return Allocate was successful or failed (mode: alloc)
     */
    bool set(size_t length,
             data_mode mode = data_mode::alloc) {
        size = length;
        alignment = align<data_ptr>();

        if (mode == data_mode::alloc)
            return allocate();

        return true;
    }

    /**
     * @brief Allocate data
     * @return Allocate was successful or failed
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

    /**
     * @brief Pointer to end of data
     * @return data_ptr    Pointer to end
     */
    data_ptr end() const {
        return ptr + size;
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
    /// Reference to const data wrapper
    using ref = cdata const&;

    /**
     * @brief Construct a new const data
     */
    cdata() = default;

    /**
     * @brief Construct a new const data
     * @param ptr       Pointer to data
     * @param length    Length of data
     */
    cdata(void const* ptr,
          size_t length)
    : ptr(as_ptr(ptr)), size(length) {}

    /**
     * @brief Construct a new const data from other data
     * @param data    Source data
     */
    cdata(data::ref data)
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
    /// Reference to unique data wrapper
    using ref = unique_data const&;

    /**
     * @brief Construct a new unique data
     * @param length    Length of data
     * @param mode      Data mode
     */
    unique_data(size_t length = 0,
                data_mode mode = data_mode::alloc) {
        if (length)
            set(length, mode);
    }

    /**
     * @brief Construct a new unique data from another data
     * @param data    Source data
     */
    explicit unique_data(data::ref data) {
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
 * @param x          Source value
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

} // namespace lava
