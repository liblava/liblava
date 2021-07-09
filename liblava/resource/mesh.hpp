/**
 * @file         liblava/resource/mesh.hpp
 * @brief        Mesh
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <iostream>
#include <liblava/resource/buffer.hpp>
#include <liblava/resource/primitive.hpp>

namespace lava {

/**
 * @brief Templated mesh data
 *
 * @tparam T Input vertex struct
 */
template<typename T = lava::vertex>
struct mesh_data {
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
    bool create(device_ptr device, bool mapped = false,
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
    void set_data(mesh_data<T> const& value) {
        data = value;
    }

    /**
     * @brief Get the mesh data
     *
     * @return mesh_data&    Mesh data
     */
    mesh_data<T>& get_data() {
        return data;
    }

    /**
     * @brief Add mesh data to existing data
     *
     * @param value    Mesh data to add
     */
    void add_data(mesh_data<T> const& value);

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
    device_ptr device = nullptr;

    /// Mesh data
    mesh_data<T> data;

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

        vkCmdBindVertexBuffers(cmd_buf, 0, to_ui32(buffers.size()), buffers.data(), buffer_offsets.data());
    }

    if (index_buffer && index_buffer->valid())
        vkCmdBindIndexBuffer(cmd_buf, index_buffer->get(), 0, VK_INDEX_TYPE_UINT32);
}

//-----------------------------------------------------------------------------
template<typename T>
void mesh_template<T>::draw(VkCommandBuffer cmd_buf) const {
    if (!data.indices.empty())
        vkCmdDrawIndexed(cmd_buf, to_ui32(data.indices.size()), 1, 0, 0, 0);
    else
        vkCmdDraw(cmd_buf, to_ui32(data.vertices.size()), 1, 0, 0);
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

//-----------------------------------------------------------------------------
/**
 * @brief Make a new mesh
 *
 * @tparam              Vertex struct typename
 * @return mesh::ptr    Shared pointer to mesh
 */
template<typename T = lava::vertex>
inline std::shared_ptr<mesh_template<T>> make_mesh() {
    return std::make_shared<mesh_template<T>>();
}

//-----------------------------------------------------------------------------
/**
 * @brief Create a new mesh
 *
 * @param device        Vulkan device
 * @param type          Mesh type
 *
 * @tparam              Vertex struct typename
 * @tparam              Position coordinate element typename
 * @tparam              Normal vector element typename
 * @tparam              If T contains a field `normal`
 * @tparam              Color vector element typename
 * @tparam              Count of elements contained in a color vector (0 if it does not exist)
 * @tparam              UV vector typename
 * @tparam              If T contains a field `color`
 *
 * @return mesh::ptr    Shared pointer to mesh
 */
// template<typename T = lava::vertex, typename PosType = float,
//          typename NormType = float, bool HasNormals = true,
//          typename ColType = float, size_t ColorComponentsCount = 4,
//          typename UVType = float, bool HasUVs = true>
template<typename T = lava::vertex, bool HasNormals = true,
         bool HasColor = true, bool HasUVs = true>
std::shared_ptr<mesh_template<T>> create_mesh(device_ptr& device,
                                              mesh_type type);

//-----------------------------------------------------------------------------
template<typename T>
bool mesh_template<T>::create(device_ptr d, bool m, VmaMemoryUsage mu) {
    device = d;
    mapped = m;
    memory_usage = mu;

    if (!data.vertices.empty()) {
        vertex_buffer = make_buffer();

        if (!vertex_buffer->create(device, data.vertices.data(),
                                   sizeof(vertex) * data.vertices.size(),
                                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                   mapped, memory_usage)) {
            log()->error("create mesh vertex buffer");
            return false;
        }
    }

    if (!data.indices.empty()) {
        index_buffer = make_buffer();

        if (!index_buffer->create(device, data.indices.data(),
                                  sizeof(ui32) * data.indices.size(),
                                  VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                  mapped, memory_usage)) {
            log()->error("create mesh index buffer");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
template<typename T, bool HasNormals,
         bool HasColor, bool HasUVs>
std::shared_ptr<mesh_template<T>> create_mesh(device_ptr& device,
                                              mesh_type type) {
    constexpr bool HasPosition2 = requires(const T& t) {
        t.position;
    };
    static_assert(HasPosition2,
                  "Vertex struct `T` must contain field `position`");
    constexpr bool HasNormals2 = requires(const T& t) {
        t.normal;
    };
    constexpr bool HasColor2 = requires(const T& t) {
        t.color;
    };
    constexpr bool HasUVs2 = requires(const T& t) {
        t.uv;
    };

    using PosType = decltype(T::position);

    auto return_mesh = make_mesh<T>();
    switch (type) {
    case mesh_type::cube: {
        return_mesh->get_indices().reserve(36);

        if constexpr (HasNormals && HasNormals2) {
            return_mesh->get_vertices().reserve(24);

            // clang-format off
            constexpr std::array<PosType, 24> positions = {{
                // Front
                { 1, 1, 1 }, { -1, 1, 1}, { -1, -1, 1 }, { 1, -1, 1 },
                // Back
                { 1, 1, -1 }, { -1, 1, -1 }, { -1, -1, -1 }, { 1, -1, -1 },
                // Left
                { -1, 1, 1 }, { -1, 1, -1 }, { -1, -1, -1 }, { -1, -1, 1 },
                // Right
                { 1, 1, 1 }, { 1, -1, 1 }, { 1, -1, -1 }, { 1, 1, -1 },
                // Bottom
                { 1, 1, 1 }, { -1, 1, 1 }, { -1, 1, -1 }, { 1, 1, -1 },
                // Top
                { 1, -1, 1 }, { -1, -1, 1 }, { -1, -1, -1 }, { 1, -1, -1 },
            }};

            // Front, back, left, right, bottom, and top normals, in that order.
            using NormType = decltype(T::normal);
            constexpr std::array<NormType, 6> normals = {{ { 0, 0, 1 },  { 0, 0, -1 },
                                                           { -1, 0, 0 }, { 1, 0, 0 },
                                                           { 0, 1, 0 },  { 0, -1, 0 }, }};

            if constexpr (HasUVs && HasUVs2) {
                using UVType = decltype(T::uv);
                constexpr std::array<UVType, 24> uvs = {{
                    // Front
                    { 1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 },
                    // Back
                    { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 },
                    // Left
                    { 1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 },
                    // Right
                    { 0, 1 }, { 0, 0 }, { 1, 0 }, { 1, 1 },
                    // Bottom
                    { 1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 1 },
                    // Top
                    { 1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 },
                }};
                // clang-format on

                // NOTE: This redundant loop is required for using uv's.
                // uvs[] cannot be uninitialized in the outer scope
                // unless -fpermissive is passed to the compiler.
                // Revisit this in the future?
                for (size_t i = 0; i < 24; i++) {
                    T vert;
                    vert.position = positions[i];
                    vert.normal = normals[i / 4];
                    vert.uv = uvs[i];
                    return_mesh->get_vertices().push_back(vert);
                }
            } else { // Does not have UV data
                for (size_t i = 0; i < 24; i++) {
                    T vert;
                    vert.position = positions[i];
                    vert.normal = normals[i / 6];
                    return_mesh->get_vertices().push_back(vert);
                }
            }

            // clang-format off
            return_mesh->get_indices() = {
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
            // clang-format on
        } else {
            // A simpler cube can be made if there are no normals.
            return_mesh->get_vertices().reserve(8);
            return_mesh->get_indices().reserve(36);

            // clang-format off
            std::array<PosType, 8> positions = { {
                { -1, -1, -1 },
                { -1, -1, 1 },
                { -1, 1, -1 },
                { -1, 1, 1 },
                { 1, -1, -1 },
                { 1, -1, 1 },
                { 1, 1, -1 },
                { 1, 1, 1 },
            } };

            // Clockwise winding order.
            return_mesh->get_indices() = {
                // Left
                0, 1, 2,
                2, 1, 3,
                // Right
                4, 5, 6,
                6, 5, 7,
                // Top
                0, 1, 4,
                4, 1, 5,
                // Bottom
                2, 3, 6,
                6, 3, 7,
                // Back
                3, 1, 5,
                5, 7, 3,
                // Front
                2, 0, 4,
                4, 6, 2,
            };
            // clang-format on

            for (size_t i = 0; i < positions.size(); i++) {
                T vert;
                vert.position = positions[i];
                return_mesh->get_vertices().push_back(vert);
            }
        }

        break;
    }

    case mesh_type::triangle: {
        return_mesh->get_vertices().reserve(3);
        T vert_one;
        vert_one.position = { 1, 1, 0 };
        T vert_two;
        vert_two.position = { -1, 1, 0 };
        T vert_three;
        vert_three.position = { 0, -1, 0 };
        if constexpr (HasNormals && HasNormals2) {
            vert_one.normal = { 1, 1, 0 };
            vert_two.normal = { -1, 1, 0 };
            vert_three.normal = { 0, -1, 0 };
        }
        return_mesh->get_vertices().push_back(vert_one);
        return_mesh->get_vertices().push_back(vert_two);
        return_mesh->get_vertices().push_back(vert_three);
        break;
    }

    case mesh_type::quad: {
        return_mesh->get_vertices().reserve(4);
        return_mesh->get_indices().reserve(6);
        T vert_one;
        vert_one.position = { 1, 1, 0 };
        T vert_two;
        vert_two.position = { -1, 1, 0 };
        T vert_three;
        vert_three.position = { -1, -1, 0 };
        T vert_four;
        vert_four.position = { 1, -1, 0 };
        if constexpr (HasUVs && HasUVs2) {
            vert_one.uv = { 1, 1 };
            vert_two.uv = { 0, 1 };
            vert_three.uv = { 0, 0 };
            vert_four.uv = { 1, 0 };
        }
        if constexpr (HasNormals && HasNormals2) {
            vert_one.normal = { 0, 0, 1 };
            vert_two.normal = { 0, 0, 1 };
            vert_three.normal = { 0, 0, 1 };
            vert_four.normal = { 0, 0, 1 };
        }
        // clang-format off
        return_mesh->get_indices() = {
            0, 1, 2,
            2, 3, 0,
        };
        // clang-format on
        return_mesh->get_vertices().push_back(vert_one);
        return_mesh->get_vertices().push_back(vert_two);
        return_mesh->get_vertices().push_back(vert_three);
        return_mesh->get_vertices().push_back(vert_four);
        break;
    }

    case mesh_type::none:
    default:
        return nullptr;
    }

    if constexpr (HasColor && HasColor2) {
        for (auto& vert : return_mesh->get_vertices()) {
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

    if (!return_mesh->create(device)) {
        return nullptr;
    }
    return return_mesh;
}

/// A mesh of lava::vertex
using mesh = mesh_template<lava::vertex>;

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
