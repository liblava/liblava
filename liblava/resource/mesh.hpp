/**
 * @file         liblava/resource/mesh.hpp
 * @brief        Vulkan mesh
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/core/hex.hpp>
#include <liblava/resource/buffer.hpp>
#include <liblava/resource/primitive.hpp>

namespace lava {

/**
 * @brief Templated mesh data
 *
 * @tparam T    Input vertex struct
 */
template<typename T = vertex>
struct mesh_template_data {
    /// List of vertices
    std::vector<T> vertices;

    /// List of indices.
    index_list indices;

public:
    /**
     * @brief Move mesh data by offset
     *
     * @tparam PosType    Coordinate element typename
     *
     * @param offset      Position offset
     */
    template<typename PosType = float>
    void move(std::array<PosType, 3> offset) {
        for (T& vertex : vertices) {
            for (auto i = 0u; i < 3; ++i) {
                vertex.position[i] += offset[i];
            }
        }
    }

    /**
     * @brief Scale mesh data by factor
     *
     * @param factor    Position scaling factor
     */
    void scale(auto factor) {
        for (T& vertex : vertices) {
            for (auto i = 0u; i < 3; ++i) {
                vertex.position[i] *= factor;
            }
        }
    }

    /**
     * @brief Scale mesh data by vector
     *
     * @tparam PosType    Coordinate element typename
     *
     * @param factors     Array of position scaling factors
     */
    template<typename PosType = float>
    void scale_vector(std::array<PosType, 3> factors) {
        for (T& vertex : vertices) {
            for (auto i = 0u; i < 3; ++i) {
                vertex.position[i] *= factors[i];
            }
        }
    }
};

/**
 * @brief Temporary templated mesh
 *
 * @tparam T    Vertex struct typename
 */
template<typename T = vertex>
struct mesh_template : entity {
    /// Shared pointer to mesh
    using ptr = std::shared_ptr<mesh_template<T>>;

    /// Map of meshes
    using map = std::map<id, ptr>;

    /// List of meshes
    using list = std::vector<ptr>;

    /// List of vertices
    using vertex_list = std::vector<T>;

    /**
     * @brief Destroy the mesh
     */
    ~mesh_template() {
        destroy();
    }

    /**
     * @brief Create a new mesh
     *
     * @param device          Vulkan device
     * @param mapped          Map mesh data
     * @param memory_usage    Memory usage
     *
     * @return true           Create was successful
     * @return false          Create failed
     */
    bool create(device_p device,
                bool mapped = false,
                VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU);

    /**
     * @brief Destroy the mesh
     */
    void destroy();

    /**
     * @brief Bind the mesh
     *
     * @param cmd_buf    Command buffer
     */
    void bind(VkCommandBuffer cmd_buf) const;

    /**
     * @brief Draw the mesh
     *
     * @param cmd_buf    Command buffer
     */
    void draw(VkCommandBuffer cmd_buf) const;

    /**
     * @brief Bind and draw the mesh
     *
     * @param cmd_buf    Command buffer
     */
    void bind_draw(VkCommandBuffer cmd_buf) const {
        bind(cmd_buf);
        draw(cmd_buf);
    }

    /**
     * @brief Check if mesh is empty
     *
     * @return true     Mesh is empty
     * @return false    Mesh is not empty
     */
    bool empty() const {
        return data.vertices.empty();
    }

    /**
     * @brief Set the mesh data
     *
     * @param value    Mesh data
     */
    void set_data(mesh_template_data<T> const& value) {
        data = value;
    }

    /**
     * @brief Get the mesh data
     *
     * @return mesh_data&    Mesh data
     */
    mesh_template_data<T>& get_data() {
        return data;
    }

    /**
     * @brief Add mesh data to existing data
     *
     * @param value    Mesh data to add
     */
    void add_data(mesh_template_data<T> const& value) {
        data = value;
    }

    /**
     * @brief Get the vertices of the mesh
     *
     * @return vertex::list&    List of vertices
     */
    vertex_list& get_vertices() {
        return data.vertices;
    }

    /**
     * @brief Get the const vertices of the mesh
     *
     * @return vertex::list const&    List of vertices
     */
    vertex_list const& get_vertices() const {
        return data.vertices;
    }

    /**
     * @brief Get the vertices count of the mesh
     *
     * @return ui32    Number of vertices
     */
    ui32 get_vertices_count() const {
        return to_ui32(data.vertices.size());
    }

    /**
     * @brief Get the indices of the mesh
     *
     * @return index_list&    List of indices
     */
    index_list& get_indices() {
        return data.indices;
    }

    /**
     * @brief Get the const indices of the mesh
     *
     * @return index_list const&    List of indices
     */
    index_list const& get_indices() const {
        return data.indices;
    }

    /**
     * @brief Get the indices count of the mesh
     *
     * @return ui32    Number of indices
     */
    ui32 get_indices_count() const {
        return to_ui32(data.indices.size());
    }

    /**
     * @brief Reload the mesh data
     *
     * @return true     Reload was successful
     * @return false    Reload failed
     */
    bool reload();

    /**
     * @brief Get the vertex buffer of the mesh
     *
     * @return buffer::ptr    Shared pointer to buffer
     */
    buffer::ptr get_vertex_buffer() {
        return vertex_buffer;
    }

    /**
     * @brief Get the index buffer of the mesh
     *
     * @return buffer::ptr    Shared pointer to buffer
     */
    buffer::ptr get_index_buffer() {
        return index_buffer;
    }

private:
    /// Vulkan device
    device_p device = nullptr;

    /// Mesh data
    mesh_template_data<T> data;

    /// Vertex buffer
    buffer::ptr vertex_buffer;

    /// Index buffer
    buffer::ptr index_buffer;

    /// Mapped state
    bool mapped = false;

    /// Memory usage
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
};

//-----------------------------------------------------------------------------
template<typename T>
void mesh_template<T>::bind(VkCommandBuffer cmd_buf) const {
    if (vertex_buffer && vertex_buffer->valid()) {
        std::array<VkDeviceSize, 1> const buffer_offsets = { 0 };
        std::array<VkBuffer, 1> const buffers = { vertex_buffer->get() };

        vkCmdBindVertexBuffers(cmd_buf, 0,
                               to_ui32(buffers.size()), buffers.data(),
                               buffer_offsets.data());
    }

    if (index_buffer && index_buffer->valid())
        vkCmdBindIndexBuffer(cmd_buf,
                             index_buffer->get(),
                             0,
                             VK_INDEX_TYPE_UINT32);
}

//-----------------------------------------------------------------------------
template<typename T>
void mesh_template<T>::draw(VkCommandBuffer cmd_buf) const {
    if (!data.indices.empty())
        vkCmdDrawIndexed(cmd_buf,
                         to_ui32(data.indices.size()),
                         1, 0, 0, 0);
    else
        vkCmdDraw(cmd_buf,
                  to_ui32(data.vertices.size()),
                  1, 0, 0);
}

//-----------------------------------------------------------------------------
template<typename T>
void mesh_template<T>::destroy() {
    vertex_buffer = nullptr;
    index_buffer = nullptr;
    device = nullptr;
}

//-----------------------------------------------------------------------------
template<typename T>
bool mesh_template<T>::reload() {
    auto dev = device;
    destroy();

    return create(dev, mapped, memory_usage);
}

/**
 * @brief Make a new mesh
 *
 * @tparam T                                    Type of vertex struct
 *
 * @return std::shared_ptr<mesh_template<T>>    Shared pointer to mesh
 */
template<typename T = vertex>
inline std::shared_ptr<mesh_template<T>> make_mesh() {
    return std::make_shared<mesh_template<T>>();
}

/**
 * @brief Create a new primitive mesh_data
 *
 * @tparam T                        Type of vertex struct
 * @tparam generate_colors          If color may be generated
 * @tparam generate_normals         If normals may be generated
 * @tparam generate_uvs             If UVs may be generated
 * @tparam has_colors               On MSVC, specifies if a `color` field exists
 * @tparam has_normals              On MSVC, specifies if a `normal` field exists
 * @tparam has_uvs                  On MSVC, specifies if a `uv` field exists
 *
 * @param type                      Mesh type
 *
 * @return mesh_template_data<T>    Mesh data
 */
template<typename T = vertex,
         bool generate_colors = true,
         bool generate_normals = true,
         bool generate_uvs = true,
         bool has_colors = true,
         bool has_normals = true,
         bool has_uvs = true>
mesh_template_data<T> create_mesh_data(mesh_type type);

/**
 * @brief Create a new primitive mesh
 *
 * @tparam T                                    Type of vertex struct
 * @tparam generate_colors                      If color may be generated
 * @tparam generate_normals                     If normals may be generated
 * @tparam generate_uvs                         If UVs may be generated
 * @tparam has_colors                           On MSVC, specifies if a `color` field exists
 * @tparam has_normals                          On MSVC, specifies if a `normal` field exists
 * @tparam has_uvs                              On MSVC, specifies if a `uv` field exists
 *
 * @param device                                Vulkan device
 * @param type                                  Mesh type
 *
 * @return std::shared_ptr<mesh_template<T>>    Shared pointer to mesh
 */
template<typename T = vertex,
         bool generate_colors = true,
         bool generate_normals = true,
         bool generate_uvs = true,
         bool has_colors = true,
         bool has_normals = true,
         bool has_uvs = true>
std::shared_ptr<mesh_template<T>> create_mesh(device_p& device,
                                              mesh_type type);

//-----------------------------------------------------------------------------
template<typename T>
bool mesh_template<T>::create(device_p d,
                              bool m,
                              VmaMemoryUsage mu) {
    device = d;
    mapped = m;
    memory_usage = mu;

    if (!data.vertices.empty()) {
        vertex_buffer = make_buffer();

        if (!vertex_buffer->create(device,
                                   data.vertices.data(),
                                   sizeof(T) * data.vertices.size(),
                                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                   mapped,
                                   memory_usage)) {
            log()->error("create mesh vertex buffer");
            return false;
        }
    }

    if (!data.indices.empty()) {
        index_buffer = make_buffer();

        if (!index_buffer->create(device,
                                  data.indices.data(),
                                  sizeof(ui32) * data.indices.size(),
                                  VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                  mapped,
                                  memory_usage)) {
            log()->error("create mesh index buffer");
            return false;
        }
    }

    return true;
}

/**
 * @brief Make primitive positions for cube
 *
 * @tparam PosType                                      Type of position
 * @tparam vert_count                                   Number of vertices
 * @tparam is_complex                                   Complex state
 * @return constexpr std::array<PosType, vert_count>    Array of positions
 */
template<typename PosType, size_t vert_count, bool is_complex>
constexpr std::array<PosType, vert_count> make_primitive_positions_cube();

//-----------------------------------------------------------------------------
// NOTE: The C++20 spec allows std::vector<T> to be constexpr
// g++ does not currently implement this feature, however
//-----------------------------------------------------------------------------

/**
 * @brief Make primitive indices for cube
 *
 * @tparam is_complex            Complex state
 * @return std::vector<index>    Array for indices
 */
template<bool is_complex>
std::vector<index> make_primitive_indices_cube();

/**
 * @brief Make primitive normals for cube
 *
 * @tparam NormType                             Type of normal
 * @return constexpr std::array<NormType, 6>    Array of normals
 */
template<typename NormType>
constexpr std::array<NormType, 6> make_primitive_normals_cube();

/**
 * @brief Make primitive uvs for cube
 *
 * @tparam UVType                              Type of uv
 * @return constexpr std::array<UVType, 24>    Array of uvs
 */
template<typename UVType>
constexpr std::array<UVType, 24> make_primitive_uvs_cube();

//-----------------------------------------------------------------------------
template<typename T,
         bool generate_colors,
         bool generate_normals,
         bool generate_uvs,
         bool has_colors,
         bool has_normals,
         bool has_uvs>
std::shared_ptr<mesh_template<T>> create_mesh(device_p& device,
                                              mesh_type type) {
    std::shared_ptr<mesh_template<T>> return_mesh =
        std::make_shared<mesh_template<T>>();

    return_mesh->add_data(create_mesh_data<T,
                                           generate_colors,
                                           generate_normals,
                                           generate_uvs,
                                           has_colors,
                                           has_normals,
                                           has_uvs>(type));
    return_mesh->create(device);
    return return_mesh;
}

//-----------------------------------------------------------------------------
template<typename T,
         bool generate_colors,
         bool generate_normals,
         bool generate_uvs,
         bool has_colors,
         bool has_normals,
         bool has_uvs>
mesh_template_data<T> create_mesh_data(mesh_type type) {
    mesh_template_data<T> return_mesh_data;

    constexpr bool auto_position = requires(const T t) {
        t.position;
    };
    static_assert(auto_position,
                  "Vertex struct `T` must contain field `position`");

    constexpr bool auto_colors = requires(const T t) {
        t.color;
    };
    constexpr bool auto_normals = requires(const T t) {
        t.normal;
    };
    constexpr bool auto_uvs = requires(const T t) {
        t.uv;
    };

    using PosType = decltype(T::position);

    switch (type) {
    case mesh_type::cube: {
        return_mesh_data.indices.reserve(36);
        return_mesh_data.indices = make_primitive_indices_cube<(generate_normals
                                                                && auto_normals)>();
        constexpr size_t vert_count = (generate_normals && auto_normals) ? 24 : 8;
        return_mesh_data.vertices.reserve(vert_count);
        auto positions = make_primitive_positions_cube<PosType, vert_count,
                                                       (generate_normals && auto_normals)>();
        for (size_t i = 0; i < vert_count; i++) {
            T vert;
            vert.position = positions[i];
            if constexpr (generate_normals && auto_normals) {
                // this array is generated inside of every loop because
                // that makes the scoping rules simplest to follow
                // my expectation is that a compiler should be able
                // to trivially optimize this
                using NormType = decltype(T::normal);
                auto normals = make_primitive_normals_cube<NormType>();
                vert.normal = normals[i / 4];
            }
            if constexpr (generate_uvs && auto_uvs) {
                using UVType = decltype(T::uv);
                auto uvs = make_primitive_uvs_cube<UVType>();
                vert.uv = uvs[i];
            }
            return_mesh_data.vertices.push_back(vert);
        }
        break;
    }

    case mesh_type::triangle: {
        return_mesh_data.vertices.reserve(3);
        T vert_one;
        vert_one.position = { 1, 1, 0 };
        T vert_two;
        vert_two.position = { -1, 1, 0 };
        T vert_three;
        vert_three.position = { 0, -1, 0 };
        if constexpr (generate_uvs && auto_uvs) {
            vert_one.uv = { 1, 1 };
            vert_two.uv = { 0, 1 };
            vert_three.uv = { 0.5, 0 };
        }
        if constexpr (generate_normals && auto_normals) {
            vert_one.normal = { 1, 1, 0 };
            vert_two.normal = { -1, 1, 0 };
            vert_three.normal = { 0, -1, 0 };
        }
        return_mesh_data.vertices.push_back(vert_one);
        return_mesh_data.vertices.push_back(vert_two);
        return_mesh_data.vertices.push_back(vert_three);
        break;
    }

    case mesh_type::quad: {
        return_mesh_data.vertices.reserve(4);
        return_mesh_data.indices.reserve(6);
        T vert_one;
        vert_one.position = { 1, 1, 0 };
        T vert_two;
        vert_two.position = { -1, 1, 0 };
        T vert_three;
        vert_three.position = { -1, -1, 0 };
        T vert_four;
        vert_four.position = { 1, -1, 0 };
        if constexpr (generate_uvs && auto_uvs) {
            vert_one.uv = { 1, 1 };
            vert_two.uv = { 0, 1 };
            vert_three.uv = { 0, 0 };
            vert_four.uv = { 1, 0 };
        }
        if constexpr (generate_normals && auto_normals) {
            vert_one.normal = { 0, 0, 1 };
            vert_two.normal = { 0, 0, 1 };
            vert_three.normal = { 0, 0, 1 };
            vert_four.normal = { 0, 0, 1 };
        }
        // clang-format off
        return_mesh_data.indices = {
            0, 1, 2,
            2, 3, 0,
        };
        // clang-format on
        return_mesh_data.vertices.push_back(vert_one);
        return_mesh_data.vertices.push_back(vert_two);
        return_mesh_data.vertices.push_back(vert_three);
        return_mesh_data.vertices.push_back(vert_four);
        break;
    }

    case mesh_type::hexagon: {
        return_mesh_data.vertices.reserve(7);
        return_mesh_data.indices.reserve(18);
        hex_layout layout;
        layout.orientation = hex_layout_point_y;
        layout.size = { 1, 1 };
        auto hex_corners = hex_polygon_corners(layout, {});
        T vert_center;
        vert_center.position = { 0, 0, 0 };
        T vert_sw;
        PosType temp;
        temp[0] = hex_corners.at(0).x;
        temp[1] = hex_corners.at(0).y;
        temp[2] = 0;
        vert_sw.position = temp;
        T vert_nw;
        temp[0] = hex_corners.at(1).x;
        temp[1] = hex_corners.at(1).y;
        vert_nw.position = temp;
        T vert_n;
        temp[0] = hex_corners.at(2).x;
        temp[1] = hex_corners.at(2).y;
        vert_n.position = temp;
        T vert_ne;
        temp[0] = hex_corners.at(3).x;
        temp[1] = hex_corners.at(3).y;
        vert_ne.position = temp;
        T vert_se;
        temp[0] = hex_corners.at(4).x;
        temp[1] = hex_corners.at(4).y;
        vert_se.position = temp;
        T vert_s;
        temp[0] = hex_corners.at(5).x;
        temp[1] = hex_corners.at(5).y;
        vert_s.position = temp;
        if constexpr (generate_uvs && auto_uvs) {
            vert_center.uv = { 0, 0 };
            vert_sw.uv = { 1, 0 };
            vert_nw.uv = { 0, 1 };
            vert_n.uv = { 1, 1 };
            vert_ne.uv = { 1, 0 };
            vert_se.uv = { 0, 1 };
            vert_s.uv = { 1, 1 };
        }
        if constexpr (generate_normals && auto_normals) {
            vert_center.normal = { 0, 0, 1 };
            vert_sw.normal = { 0, 0, 1 };
            vert_nw.normal = { 0, 0, 1 };
            vert_n.normal = { 0, 0, 1 };
            vert_ne.normal = { 0, 0, 1 };
            vert_se.normal = { 0, 0, 1 };
            vert_s.normal = { 0, 0, 1 };
        }
        // clang-format off
        return_mesh_data.indices = {
            0, 1, 6, 0, 6, 5, 0, 5, 4, 0, 4, 3, 0, 3, 2, 0, 2, 1
        };
        // clang-format on
        return_mesh_data.vertices.push_back(vert_center);
        return_mesh_data.vertices.push_back(vert_sw);
        return_mesh_data.vertices.push_back(vert_nw);
        return_mesh_data.vertices.push_back(vert_n);
        return_mesh_data.vertices.push_back(vert_ne);
        return_mesh_data.vertices.push_back(vert_se);
        return_mesh_data.vertices.push_back(vert_s);
        break;
    }

    case mesh_type::none:
    default:
        break;
    }

    if constexpr (generate_colors && auto_colors) {
        for (auto& vert : return_mesh_data.vertices) {
            // this does not work on glm vectors
            // for (auto& this_color : vert.color) {
            //     for (auto& color_component : this_color) {
            //         color_component = 1;
            //     }
            // }
            if constexpr (std::is_same<decltype(vert.color), glm::vec3>::value) {
                vert.color = { 1, 1, 1 };
            } else if constexpr (std::is_same<decltype(vert.color), glm::vec4>::value) {
                vert.color = { 1, 1, 1, 1 };
            } else {
                for (size_t i = 0; i < vert.color.size(); i++) {
                    vert.color[i] = 1;
                }
            }
        }
    }

    return return_mesh_data;
}

//-----------------------------------------------------------------------------
template<typename PosType,
         size_t vert_count,
         bool is_complex>
constexpr std::array<PosType, vert_count> make_primitive_positions_cube() {
    if constexpr (is_complex) {
        // clang-format off
        constexpr std::array<PosType, 24> positions = {{
            // front
            { 1, 1, 1 }, { -1, 1, 1}, { -1, -1, 1 }, { 1, -1, 1 },
            // back
            { 1, 1, -1 }, { -1, 1, -1 }, { -1, -1, -1 }, { 1, -1, -1 },
            // left
            { -1, 1, 1 }, { -1, 1, -1 }, { -1, -1, -1 }, { -1, -1, 1 },
            // right
            { 1, 1, 1 }, { 1, -1, 1 }, { 1, -1, -1 }, { 1, 1, -1 },
            // bottom
            { 1, 1, 1 }, { -1, 1, 1 }, { -1, 1, -1 }, { 1, 1, -1 },
            // top
            { 1, -1, 1 }, { -1, -1, 1 }, { -1, -1, -1 }, { 1, -1, -1 },
        }};
        return positions;
    } else {
        constexpr std::array<PosType, 8> positions = {{
            { -1, -1, -1 },
            { -1, -1, 1 },
            { -1, 1, -1 },
            { -1, 1, 1 },
            { 1, -1, -1 },
            { 1, -1, 1 },
            { 1, 1, -1 },
            { 1, 1, 1 },
        }};
        return positions;
    }
    // clang-format on
}

//-----------------------------------------------------------------------------
template<bool is_complex>
std::vector<index> make_primitive_indices_cube() {
    // clang-format off
    if constexpr (is_complex) {
        std::vector<index> indices = {
            0, 1, 2,
            2, 3, 0,
            4, 7, 6,
            6, 5, 4,
            8, 9, 10,
            10, 11, 8,
            12, 13, 14,
            14, 15, 12,
            16, 19, 18,
            18, 17, 16,
            20, 21, 22,
            22, 23, 20,
        };
        return indices;
    } else {
        // clockwise winding order
        std::vector<index> indices = {
            // left
            0, 1, 2,
            2, 1, 3,
            // right
            4, 5, 6,
            6, 5, 7,
            // top
            0, 1, 4,
            4, 1, 5,
            // bottom
            2, 3, 6,
            6, 3, 7,
            // back
            3, 1, 5,
            5, 7, 3,
            // front
            2, 0, 4,
            4, 6, 2,
        };
        return indices;
    }
    // clang-format on
}

//-----------------------------------------------------------------------------
template<typename NormType>
constexpr std::array<NormType, 6> make_primitive_normals_cube() {
    // clang-format off
    // front, back, left, right, bottom, and top normals, in that order
    constexpr std::array<NormType, 6> normals = {{ { 0, 0, 1 },  { 0, 0, -1 },
                                                   { -1, 0, 0 }, { 1, 0, 0 },
                                                   { 0, 1, 0 },  { 0, -1, 0 }, }};
    // clang-format on
    return normals;
}

//-----------------------------------------------------------------------------
template<typename UVType>
constexpr std::array<UVType, 24> make_primitive_uvs_cube() {
    // clang-format off
    constexpr std::array<UVType, 24> uvs = {{
        { 1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 }, // front
        { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 }, // back
        { 1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 }, // left
        { 0, 1 }, { 0, 0 }, { 1, 0 }, { 1, 1 }, // right
        { 1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 1 }, // bottom
        { 1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 }, // top
    }};
    // clang-format on
    return uvs;
}

/// Mesh data with default vertex
using mesh_data = mesh_template_data<vertex>;

/// Mesh with default vertex
using mesh = mesh_template<vertex>;

/**
 * @brief Mesh meta
 */
struct mesh_meta {
    /// Name of file (empty: see type)
    string filename;

    /// Mesh type
    mesh_type type = mesh_type::none;
};

/// Mesh registry
using mesh_registry = id_registry<mesh, mesh_meta>;

} // namespace lava
