/**
 * @file         liblava/resource/primitive.hpp
 * @brief        Vulkan primitives
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/core/math.hpp"

namespace lava {

/**
 * @brief Vertex
 */
struct vertex {
    /// List of vertices
    using list = std::vector<vertex>;

    /// Vertex position
    v3 position;

    /// Vertex color
    v4 color;

    /// Vertex uv
    v2 uv;

    /// Vertex normal
    v3 normal;

    /**
     * @brief Equal compare operator
     *
     * @param other     Another vertex
     *
     * @return true     Another vertex is equal
     * @return false    Another vertex is unequal
     */
    bool operator==(vertex const& other) const {
        return position == other.position && color == other.color && uv == other.uv && normal == other.normal;
    }
};

/**
 * @brief Mesh types
 */
enum class mesh_type : type {
    none = 0,
    cube,
    triangle,
    quad
};

} // namespace lava
