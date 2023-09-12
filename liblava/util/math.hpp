/**
 * @file         liblava/util/math.hpp
 * @brief        Math helpers
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/core/types.hpp"
#include "picosha2.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace lava {

/// Vector 2D
using v2 = glm::vec2;

/// Vector 3D
using v3 = glm::vec3;

/// Vector 4D
using v4 = glm::vec4;

/// UV pair
using uv2 = glm::uvec2;

/// Matrix 3x3
using mat3 = glm::mat3;

/// Matrix 4x4
using mat4 = glm::mat4;

/// Integer vector 2D
using iv2 = glm::ivec2;

/// Integer vector 3D
using iv3 = glm::ivec3;

/**
 * @brief Rectangle
 */
struct rect {
    /// Reference to rect
    using ref = rect const&;

    /**
     * @brief Construct a new rectangle
     */
    rect() = default;

    /**
     * @brief Construct a new rectangle
     * @param left      Left position
     * @param top       Top position
     * @param width     Rectangle width
     * @param height    Rectangle height
     */
    rect(i32 left, i32 top,
         ui32 width, ui32 height)
    : left_top({ left, top }) {
        set_size({ width, height });
    }

    /**
     * @brief Construct a new rectangle
     * @param left_top    Left top position
     * @param width       Rectangle width
     * @param height      Rectangle height
     */
    rect(iv2 const& left_top,
         ui32 width, ui32 height)
    : left_top(left_top) {
        set_size({ width, height });
    }

    /**
     * @brief Construct a new rectangle
     * @param left_top    Left top position
     * @param size        Size of rectangle
     */
    rect(iv2 const& left_top,
         uv2 const& size)
    : left_top(left_top) {
        set_size(size);
    }

    /**
     * @brief Get the origin
     * @return iv2 const&    Left top position
     */
    iv2 const& get_origin() const {
        return left_top;
    }

    /**
     * @brief Get the end point
     * @return iv2 const&    Right bottom position
     */
    iv2 const& get_end_point() const {
        return right_bottom;
    }

    /**
     * @brief Get the size
     * @return uv2    Width and height
     */
    uv2 get_size() const {
        LAVA_ASSERT(left_top.x <= right_bottom.x);
        LAVA_ASSERT(left_top.y <= right_bottom.y);
        return { right_bottom.x - left_top.x,
                 right_bottom.y - left_top.y };
    }

    /**
     * @brief Set the size
     * @param size    Width and height
     */
    void set_size(uv2 const& size) {
        right_bottom.x = left_top.x + size.x;
        right_bottom.y = left_top.y + size.y;
    }

    /**
     * @brief Move the rectangle
     * @param offset    Offset to move
     */
    void move(iv2 const& offset) {
        left_top += offset;
        right_bottom += offset;
    }

    /**
     * @brief Check if point is inside the rectangle
     * @param point     Point to check
     * @return Point is inside or out
     */
    bool contains(iv2 point) const {
        return (left_top.x < point.x)
               && (left_top.y < point.y)
               && (right_bottom.x > point.x)
               && (right_bottom.y > point.y);
    }

private:
    /// Left top position
    iv2 left_top = iv2();

    /// Right bottom position
    iv2 right_bottom = iv2();
};

/**
 * @brief Ceiling of division
 * @param x        X value
 * @param y        Y value
 * @return auto    Result
 */
inline auto ceil_div(auto x, auto y) {
    return (x + y - 1) / y;
}

/// Default color (Lava color: CF1020 : 207, 16, 32)
constexpr v3 const default_color = v3{ 0.8118f,
                                       0.0627f,
                                       0.1255f };

/**
 * @brief Calculate perspective matrix
 * @param size         Size for aspect ratio
 * @param fov          Field of view
 * @param far_plane    Far plane
 * @return mat4        Calculated matrix
 */
inline mat4 perspective_matrix(uv2 size,
                               r32 fov = 90.f,
                               r32 far_plane = 5.f) {
    // Vulkan NDC is right-handed with Y pointing down
    // we flip Y which makes it left-handed
    return glm::scale(glm::identity<glm::mat4>(),
                      { 1.f, -1.f, 1.f })
           * glm::perspectiveLH_ZO(
               glm::radians(fov),
               r32(size.x) / size.y,
               0.1f,
               far_plane);
}

/**
 * @brief Get SHA-256 hash of string
 * @param value      Value to hash
 * @return string    Hash result
 */
inline string hash256(string_ref value) {
    std::vector<uc8> hash(picosha2::k_digest_size);
    picosha2::hash256(value.begin(), value.end(),
                      hash.begin(), hash.end());

    return picosha2::bytes_to_hex_string(hash.begin(), hash.end());
}

} // namespace lava
