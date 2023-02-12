/**
 * @file         liblava/util/hex.hpp
 * @brief        Hex point, cell and grid
 * @see          https://www.redblobgames.com/grids/hexagons/
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <cmath>
#include <numbers>
#include <unordered_map>
#include "liblava/core/types.hpp"

namespace lava {

/**
 * @brief Hex point
 */
struct hex_point {
    /// List of hex points
    using list = std::vector<hex_point>;

    /// X coordinate
    r32 x{};

    /// Y coordinate
    r32 y{};
};

/**
 * @brief Hex cell
 */
struct hex_cell {
    /// List of hex cells
    using list = std::vector<hex_cell>;

    /// Q axis
    i32 q{};

    /// R axis
    i32 r{};

    /// S axis
    i32 s{};

    /**
     * @brief Compare operator
     */
    auto operator<=>(hex_cell const&) const = default;

    /// Hex pair (Q and R)
    using pair = std::pair<i32, i32>;

    /// Map of hex cells
    using map = std::unordered_map<pair, index, pair_hash>;

    /**
     * @brief Get the pair
     * @return pair    Hex pair
     */
    inline pair to_pair() const {
        return { q, r };
    }

    /**
     * @brief Add hex cell
     * @param cell    Another hex cell
     */
    inline void add(hex_cell const& cell) {
        *this = { q + cell.q,
                  r + cell.r,
                  s + cell.s };
    }

    /**
     * @brief Substract hex cell
     * @param cell    Another hex cell
     */
    inline void substract(hex_cell const& cell) {
        *this = { q - cell.q,
                  r - cell.r,
                  s - cell.s };
    }

    /**
     * @brief Scale the hex cell
     * @param factor    Scaling factor
     */
    inline void scale(i32 factor) {
        *this = { q * factor,
                  r * factor,
                  s * factor };
    }

    /**
     * @brief Rotate to left
     */
    inline void rotate_left() {
        *this = { -s, -q, -r };
    }

    /**
     * @brief Rotate to right
     */
    inline void rotate_right() {
        *this = { -r, -s, -q };
    }
};

/**
 * @brief Get S axis from Q and R axes
 * @param q       Q axis
 * @param r       R axis
 * @return i32    S axis
 */
inline i32 hex_get_s(i32 q, i32 r) {
    return -q - r;
}

/**
 * @brief Get hex cell from pair
 * @param pair         Hex pair
 * @return hex_cell    Hex cell
 */
inline hex_cell hex_cell_from_pair(hex_cell::pair const& pair) {
    return { pair.first, pair.second,
             hex_get_s(pair.first, pair.second) };
}

/**
 * @brief Hex fractional cell
 */
struct hex_fractional_cell {
    /// Q axis
    r32 q{};

    /// R axis
    r32 r{};

    /// S axis
    r32 s{};
};

/// Hex fractional cell
using hex_frac_cell = hex_fractional_cell;

/**
 * @brief Check if hex cell is valid
 * @param cell      Hex cell to check
 * @return Hex cell is valid or not
 */
inline bool hex_is_valid(hex_cell const& cell) {
    return cell.q + cell.r + cell.s == 0;
}

/**
 * @brief Hex offset coordinates
 */
struct hex_offset_coord {
    /// Column coordinate
    i32 col{};

    /// Row coordinate
    i32 row{};
};

/// Hex doubled coordinates
using hex_doubled_coord = hex_offset_coord;

/**
 * @brief Hex orientation
 */
struct hex_orientation {
    /// F0 value
    r32 f0{};

    /// F1 value
    r32 f1{};

    /// F2 value
    r32 f2{};

    /// F3 value
    r32 f3{};

    /// B0 value
    r32 b0{};

    /// B1 value
    r32 b1{};

    /// B2 value
    r32 b2{};

    /// B3 value
    r32 b3{};

    /// Start angle
    r32 start_angle{};
};

/**
 * @brief Hex layout
 */
struct hex_layout {
    /// Hex orientation
    hex_orientation orientation;

    /// Hex origin
    hex_point origin;

    /// Hex size
    hex_point size;
};

/// List of hex directions
hex_cell::list const hex_directions{
    { 1, 0, -1 },
    { 1, -1, 0 },
    { 0, -1, 1 },
    { -1, 0, 1 },
    { -1, 1, 0 },
    { 0, 1, -1 }
};

/**
 * @brief Get the hex cell from direction
 * @param direction    Direction index
 * @return hex_cell    Hex cell
 */
inline hex_cell hex_direction(index direction) {
    return hex_directions[direction];
}

/**
 * @brief Get the neighbor of hex cell by direction
 * @param cell         Target hex cell
 * @param direction    Direction index
 * @return hex_cell    Neighbor hex cell
 */
inline hex_cell hex_neighbor(hex_cell const& cell,
                             index direction) {
    auto neighbor = cell;
    neighbor.add(hex_direction(direction));
    return neighbor;
}

/// List of hex diagonals
hex_cell::list const hex_diagonals{
    { 2, -1, -1 },
    { -1, -2, 1 },
    { -1, -1, 2 },
    { -2, 1, 1 },
    { -1, 2, -1 },
    { 1, 1, -2 }
};

/**
 * @brief Get the diagonal from direction
 * @param direction    Direction index
 * @return hex_cell    Hex cell
 */
inline hex_cell hex_diagonal(index direction) {
    return hex_diagonals[direction];
}

/**
 * @brief Get the diagonal neighbor of hex cell by direction
 * @param cell         Target hex cell
 * @param direction    Direction index
 * @return hex_cell    Diagonal neighbor hex cell
 */
inline hex_cell hex_diagonal_neighbor(hex_cell const& cell,
                                      index direction) {
    auto neighbor = cell;
    neighbor.add(hex_diagonal(direction));
    return neighbor;
}

/**
 * @brief Get the length of hex cell
 * @param cell    Target hex cell
 * @return i32    Length of hex cell
 */
inline i32 hex_length(hex_cell const& cell) {
    return to_i32(std::abs(cell.q)
                  + std::abs(cell.r)
                  + std::abs(cell.s) / 2);
}

/**
 * @brief Get the distance between 2 hex cells
 * @param a       Source hex cell
 * @param b       Target hex cell
 * @return i32    Distance
 */
inline i32 hex_distance(hex_cell const& a,
                        hex_cell const& b) {
    auto dist = a;
    dist.substract(b);
    return hex_length(dist);
}

/**
 * @brief Round a fractional cell to hex cell
 * @param cell         Target fractional cell
 * @return hex_cell    Rounded hex cell
 */
inline hex_cell hex_round(hex_frac_cell const& cell) {
    auto qi = to_i32(std::round(cell.q));
    auto ri = to_i32(std::round(cell.r));
    auto si = to_i32(std::round(cell.s));

    auto const q_diff = std::abs(qi - cell.q);
    auto const r_diff = std::abs(ri - cell.r);
    auto const s_diff = std::abs(si - cell.s);

    if ((q_diff > r_diff) && (q_diff > s_diff))
        qi = -ri - si;
    else if (r_diff > s_diff)
        ri = -qi - si;
    else
        si = -qi - ri;

    return { qi, ri, si };
}

/**
 * @brief Get the linear interpolation between 2 hex cells
 * @param a                 Source fractional hex cell
 * @param b                 Target fractional hex cell
 * @param t                 Factor
 * @return hex_frac_cell    Fractional hex cell
 */
inline hex_frac_cell hex_lerp(hex_frac_cell const& a,
                              hex_frac_cell const& b,
                              r32 t) {
    return {
        a.q * (1.f - t) + b.q * t,
        a.r * (1.f - t) + b.r * t,
        a.s * (1.f - t) + b.s * t
    };
}

/**
 * @brief Get the line between 2 hex cells
 * @param a                  Source hex cell
 * @param b                  Target hex cell
 * @return hex_cell::list    List of hex cells
 */
inline hex_cell::list hex_line(hex_cell const& a,
                               hex_cell const& b) {
    auto const a_nudge = hex_frac_cell{ a.q + 0.000001f,
                                        a.r + 0.000001f,
                                        a.s - 0.000002f };
    auto const b_nudge = hex_frac_cell{ b.q + 0.000001f,
                                        b.r + 0.000001f,
                                        b.s - 0.000002f };

    ui32 const n = hex_distance(a, b);
    auto const step = 1.0 / std::max(n, 1u);

    hex_cell::list results;
    for (auto i = 0u; i <= n; ++i)
        results.push_back(hex_round(hex_lerp(a_nudge,
                                             b_nudge,
                                             step * i)));

    return results;
}

/**
 * @brief Hex offsets
 */
enum class hex_offset : i32 {
    odd = -1,
    even = 1
};

/**
 * @brief Get the Q offset from hex cube
 * @param offset               Hex offset
 * @param cell                 Hex cell
 * @return hex_offset_coord    Hex offset coordinates
 */
inline hex_offset_coord hex_q_offset_from_cube(hex_offset offset,
                                               hex_cell const& cell) {
    auto const& col = cell.q;
    auto const row = cell.r
                     + to_i32((cell.q + (i32) offset * (cell.q & 1)) / 2);
    return { col, row };
}

/**
 * @brief Get the Q offset to hex cube
 * @param offset       Hex offset
 * @param coord        Hex offset coordinates
 * @return hex_cell    Hex cell
 */
inline hex_cell hex_q_offset_to_cube(hex_offset offset,
                                     hex_offset_coord const& coord) {
    auto const& q = coord.col;
    auto const r = coord.row
                   - to_i32((coord.col + (i32) offset * (coord.col & 1)) / 2);
    auto const s = -q - r;
    return { q, r, s };
}

/**
 * @brief Get the R offset from hex cube
 * @param offset               Hex offset
 * @param cell                 Hex cell
 * @return hex_offset_coord    Hex offset coordinates
 */
inline hex_offset_coord hex_r_offset_from_cube(hex_offset offset,
                                               hex_cell const& cell) {
    auto const col = cell.q
                     + to_i32((cell.r + (i32) offset * (cell.r & 1)) / 2);
    auto const& row = cell.r;
    return { col, row };
}

/**
 * @brief Get the R offset to hex cube
 * @param offset       Hex offset
 * @param coord        Hex offset coordinates
 * @return hex_cell    Hex cell
 */
inline hex_cell hex_r_offset_to_cube(hex_offset offset,
                                     hex_offset_coord const& coord) {
    auto const q = coord.col
                   - to_i32((coord.row + (i32) offset * (coord.row & 1)) / 2);
    auto const& r = coord.row;
    auto const s = -q - r;
    return { q, r, s };
}

/**
 * @brief Get the Q doubled from hex cube
 * @param cell                  Hex cell
 * @return hex_doubled_coord    Hex doubled coordinates
 */
inline hex_doubled_coord hex_q_doubled_from_cube(hex_cell const& cell) {
    auto const& col = cell.q;
    auto const row = 2 * cell.r + cell.q;
    return { col, row };
}

/**
 * @brief Get the Q doubled to hex cube
 * @param coord        Hex doubled coordinates
 * @return hex_cell    Hex cell
 */
inline hex_cell hex_q_doubled_to_cube(hex_doubled_coord const& coord) {
    auto const& q = coord.col;
    auto const r = to_i32((coord.row - coord.col) / 2);
    auto const s = -q - r;
    return { q, r, s };
}

/**
 * @brief Get the R offset from hex cube
 * @param cell                  Hex cell
 * @return hex_doubled_coord    Hex doubled coordinates
 */
inline hex_doubled_coord hex_r_doubled_from_cube(hex_cell const& cell) {
    auto const col = 2 * cell.q + cell.r;
    auto const& row = cell.r;
    return { col, row };
}

/**
 * @brief Get the R doubled to hex cube
 * @param coord        Hex doubled coordinates
 * @return hex_cell    Hex cell
 */
inline hex_cell hex_r_doubled_to_cube(hex_doubled_coord const& coord) {
    auto const q = to_i32((coord.col - coord.row) / 2);
    auto const& r = coord.row;
    auto const s = -q - r;
    return { q, r, s };
}

/// Hex point Y layout
hex_orientation const hex_layout_point_y = {
    std::sqrt(3.f),
    std::sqrt(3.f) / 2.f,
    0.f,
    3.f / 2.f,
    std::sqrt(3.f) / 3.f,
    -1.f / 3.f,
    0.f,
    2.f / 3.f,
    0.5f
};

/// Hex flat layout
hex_orientation const hex_layout_flat = {
    3.f / 2.f,
    0.f,
    std::sqrt(3.f) / 2.f,
    std::sqrt(3.f),
    2.f / 3.f,
    0.f,
    -1.f / 3.f,
    std::sqrt(3.f) / 3.f,
    0.f
};

/**
 * @brief Convert the hex cell to pixel
 * @param layout        Hex layout
 * @param cell          Hex cell
 * @return hex_point    Hex point
 */
inline hex_point hex_to_pixel(hex_layout const& layout,
                              hex_cell const& cell) {
    auto const& m = layout.orientation;
    auto const& size = layout.size;
    auto const& origin = layout.origin;
    auto const x = (m.f0 * cell.q + m.f1 * cell.r) * size.x;
    auto const y = (m.f2 * cell.q + m.f3 * cell.r) * size.y;
    return { x + origin.x, y + origin.y };
}

/**
 * @brief Convert the hex point to cell
 * @param layout            Hex layout
 * @param p                 Hex point
 * @return hex_frac_cell    Hex fractional cell
 */
inline hex_frac_cell hex_pixel_to_cell(hex_layout const& layout,
                                       hex_point const& p) {
    auto const& m = layout.orientation;
    auto const& size = layout.size;
    auto const& origin = layout.origin;
    auto const pt = hex_point{ (p.x - origin.x) / size.x,
                               (p.y - origin.y) / size.y };
    auto const q = m.b0 * pt.x + m.b1 * pt.y;
    auto const r = m.b2 * pt.x + m.b3 * pt.y;
    return { q, r, -q - r };
}

/**
 * @brief Get the hex corner offset
 * @param layout        Hex layout
 * @param corner        Corner
 * @return hex_point    Hex point
 */
inline hex_point hex_corner_offset(hex_layout const& layout,
                                   i32 corner) {
    auto const& m = layout.orientation;
    auto const& size = layout.size;
    auto const angle = 2.
                       * std::numbers::pi_v<r32> * (m.start_angle - corner)
                       / 6.f;
    return { size.x * (r32) std::cos(angle),
             size.y * (r32) std::sin(angle) };
}

/**
 * @brief Get the hex polygon corners
 * @param layout              Hex layout
 * @param cell                Hex cell
 * @return hex_point::list    List of hex points
 */
inline hex_point::list hex_polygon_corners(hex_layout const& layout,
                                           hex_cell const& cell) {
    hex_point::list corners = {};
    auto const center = hex_to_pixel(layout, cell);
    for (int i = 0u; i < 6; ++i) {
        auto const offset = hex_corner_offset(layout, i);
        corners.push_back({ center.x + offset.x,
                            center.y + offset.y });
    }
    return corners;
}

/**
 * @brief Get the hex point by corner
 * @param center        Center hex point
 * @param size          Size of hex cell
 * @param corner        Corner
 * @return hex_point    Hex point
 */
inline hex_point hex_get_corner(hex_point const& center,
                                r32 size,
                                ui32 corner) {
    auto const angle_deg = 60 * corner - 30;
    auto const angle_rad = std::numbers::pi_v<r32> / 180 * angle_deg;

    return {
        center.x + size * std::cos(angle_rad),
        center.y + size * std::sin(angle_rad)
    };
}

/**
 * @brief Hex cardinal directions
 */
enum class hex_cardinal_direction : index {
    NE = 0,
    E,
    SE,
    SW,
    W,
    NW
};

/**
 * @brief Convert hex cardinal direction to string
 * @param direction    Hex cardinal direction
 * @return string      String representation
 */
inline string to_string(hex_cardinal_direction direction) {
    switch (direction) {
    case hex_cardinal_direction::NE: {
        return "Northeast";
    }
    case hex_cardinal_direction::E: {
        return "East";
    }
    case hex_cardinal_direction::SE: {
        return "Southeast";
    }
    case hex_cardinal_direction::SW: {
        return "Southwest";
    }
    case hex_cardinal_direction::W: {
        return "West";
    }
    case hex_cardinal_direction::NW: {
        return "Northwest";
    }
    }
}

/// List of hex cardinal directions
hex_cell::list const hex_cardinal_directions{
    { 1, 0, -1 },
    { 0, 1, -1 },
    { -1, 1, 0 },
    { -1, 0, 1 },
    { 0, -1, 1 },
    { 1, -1, 0 }
};

/**
 * @brief Get the hex cell from cardinal direction
 * @param direction    Hex cardinal direction
 * @return hex_cell    Hex cell
 */
inline hex_cell hex_get(hex_cardinal_direction direction) {
    return hex_cardinal_directions[(index) direction];
}

/**
 * @brief Get the opposite cardinal direction
 * @param direction                  Hex cardinal direction
 * @return hex_cardinal_direction    Hex cardinal direction
 */
inline hex_cardinal_direction hex_opposite(hex_cardinal_direction direction) {
    if ((index) direction < 3)
        return hex_cardinal_direction((i32) direction + 3);
    else
        return hex_cardinal_direction((i32) direction - 3);
}

/// Hex inner radius factor = sqrt(3) / 2
constexpr r32 const hex_inner_radius_factor = 0.866025404f;

/// Hex default outer radius
constexpr r32 const hex_default_outer_radius = 1.f;

/**
 * @brief Get the hex inner radius from outer radius
 * @param outer_radius    Hex outer radius
 * @return r32            Hex inner radius
 */
inline r32 hex_calculate_inner_radius(r32 outer_radius) {
    return outer_radius * hex_inner_radius_factor;
}

/**
 * @brief Hex grid
 */
struct hex_grid {
    /// Hex inner radius
    r32 inner_radius = 0.f;

    /// Hex outer radius
    r32 outer_radius = hex_default_outer_radius;

    /// Hex layout
    hex_layout layout;

    /**
     * @brief Construct a new hex grid
     * @param radius    Hex outer radius
     */
    hex_grid(r32 radius = hex_default_outer_radius)
    : outer_radius(radius) {
        update();
    }

    /**
     * @brief Update the hex grid
     * @param orientation    Hex orientation
     */
    void update(hex_orientation orientation = hex_layout_point_y) {
        inner_radius = hex_calculate_inner_radius(outer_radius);
        layout = {
            orientation,
            {},
            { outer_radius, outer_radius }
        };
    }

    /**
     * @brief Find the hex cell from X and Y coordinates
     * @param x            X coordinate
     * @param y            Y coordinate
     * @return hex_cell    Hex cell
     */
    hex_cell find(r32 x, r32 y) const {
        return hex_round(hex_pixel_to_cell(layout,
                                           { x, y }));
    }

    /**
     * @brief Get the hex point from hex cell
     * @param cell          Hex cell
     * @return hex_point    Hex point
     */
    hex_point to_pixel(hex_cell const& cell) const {
        return hex_to_pixel(layout, cell);
    }
};

} // namespace lava
