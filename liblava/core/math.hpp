// file      : liblava/core/math.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/types.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace lava {

using v2 = glm::vec2;
using v3 = glm::vec3;
using v4 = glm::vec4;

using uv2 = glm::uvec2;

using mat3 = glm::mat3;
using mat4 = glm::mat4;

using iv2 = glm::ivec2;
using iv3 = glm::ivec3;

struct rect {

    rect() = default;

    rect(i32 left, i32 top, ui32 width, ui32 height) : left_top({ left, top }) {

        set_size({ width, height });
    }

    rect(iv2 const& left_top, ui32 width, ui32 height) : left_top(left_top) {

        set_size({ width, height });
    }

    rect(iv2 const& left_top, uv2 const& size) : left_top(left_top) {

        set_size(size);
    }

    iv2 const& get_origin() const { return left_top; }
    iv2 const& get_end_point() const { return right_bottom; }

    uv2 get_size() const {

        assert(left_top.x <= right_bottom.x);
        assert(left_top.y <= right_bottom.y);
        return { right_bottom.x - left_top.x, right_bottom.y - left_top.y };
    }

    void set_size(uv2 const& size) {

        right_bottom.x = left_top.x + size.x;
        right_bottom.y = left_top.y + size.y;
    }

    void move(iv2 const& offset) {

        left_top += offset;
        right_bottom += offset;
    }

    bool contains(iv2 point) const {

        return (left_top.x < point.x) && (left_top.y < point.y) && 
                (right_bottom.x > point.x) && (right_bottom.y > point.y);
    }

private:
    iv2 left_top = iv2();
    iv2 right_bottom = iv2();
};

template <typename T>
inline T ceil_div(T x, T y) { return (x + y - 1) / y; }

v3 const default_color = v3{ 0.8118f, 0.0627f, 0.1255f }; // #CF1020 : 207, 16, 32

} // lava
