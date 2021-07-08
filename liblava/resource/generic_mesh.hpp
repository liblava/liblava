/**
 * @file         liblava/resource/generic_mesh.hpp
 * @brief        Generic mesh
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
struct generic_mesh_data {
    /// List of vertices
    std::vector<T> vertices;

    /// List of indices.
    index_list indices;

public:
    /**
     * @brief Move mesh data by offset
     *
     * @tparam PosType    Type of coordinate element
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
 * @tparam T    Input vertex struct
 */
template<typename T = vertex>
struct generic_mesh : entity {
    using ptr = std::shared_ptr<generic_mesh<T>>;
    using map = std::map<id, ptr>;
    using list = std::vector<ptr>;
    using vertex_list = std::vector<T>;
    ~generic_mesh() {
        destroy();
    }
    bool create(device_ptr device, bool mapped = false, VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU);
    void destroy();
    void bind(VkCommandBuffer cmd_buf) const;
    void draw(VkCommandBuffer cmd_buf) const;
    void bind_draw(VkCommandBuffer cmd_buf) const {
        bind(cmd_buf);
        draw(cmd_buf);
    }
    bool empty() const {
        return data.vertices.empty();
    }
    void set_data(generic_mesh_data<T> const& value) {
        data = value;
    }
    generic_mesh_data<T>& get_data() {
        return data;
    }
    void add_data(generic_mesh_data<T> const& value);
    vertex_list& get_vertices() {
        return data.vertices;
    }
    vertex_list const& get_vertices() const {
        return data.vertices;
    }
    ui32 get_vertices_count() const {
        return to_ui32(data.vertices.size());
    }
    index_list& get_indices() {
        return data.indices;
    }
    index_list const& get_indices() const {
        return data.indices;
    }
    ui32 get_indices_count() const {
        return to_ui32(data.indices.size());
    }
    bool reload();
    buffer::ptr get_vertex_buffer() {
        return vertex_buffer;
    }
    buffer::ptr get_index_buffer() {
        return index_buffer;
    }

private:
    device_ptr device = nullptr;
    generic_mesh_data<T> data;
    buffer::ptr vertex_buffer;
    buffer::ptr index_buffer;
    bool mapped = false;
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
};

template<typename T>
void generic_mesh<T>::bind(VkCommandBuffer cmd_buf) const {
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
void generic_mesh<T>::draw(VkCommandBuffer cmd_buf) const {
    if (!data.indices.empty())
        vkCmdDrawIndexed(cmd_buf, to_ui32(data.indices.size()), 1, 0, 0, 0);
    else
        vkCmdDraw(cmd_buf, to_ui32(data.vertices.size()), 1, 0, 0);
}

//-----------------------------------------------------------------------------
template<typename T>
void generic_mesh<T>::destroy() {
    vertex_buffer = nullptr;
    index_buffer = nullptr;

    device = nullptr;
}

//-----------------------------------------------------------------------------
template<typename T>
bool generic_mesh<T>::reload() {
    auto dev = device;
    destroy();

    return create(dev, mapped, memory_usage);
}

//-----------------------------------------------------------------------------
template<typename T>
inline std::shared_ptr<generic_mesh<T>> generic_make_mesh() {
    return std::make_shared<generic_mesh<T>>();
}

//-----------------------------------------------------------------------------
template<typename T = lava::vertex, typename PosType = float,
         typename NormType = float, bool HasNormals = true,
         typename ColType = float, size_t ColorComponentsCount = 4,
         typename UVType = float, bool HasUVs = true>
std::shared_ptr<generic_mesh<T>> generic_create_mesh(device_ptr& device,
                                                     mesh_type type);

//-----------------------------------------------------------------------------
template<typename T>
bool generic_mesh<T>::create(device_ptr d, bool m, VmaMemoryUsage mu) {
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
template<typename T, typename PosType, typename NormType, bool HasNormals,
         typename ColType, size_t ColorComponentsCount, typename UVType,
         bool HasUVs>
std::shared_ptr<generic_mesh<T>> generic_create_mesh(device_ptr& device,
                                                     mesh_type type) {
    auto set_color = [&](T & vert) constexpr {
        for (size_t i = 0; i < ColorComponentsCount; i++) {
            vert.color[i] = 1;
        }
    };

    auto return_mesh = generic_make_mesh<T>();
    switch (type) {
    case mesh_type::cube: {
        return_mesh->get_indices().reserve(36);

        if constexpr (HasNormals) {
            return_mesh->get_vertices().reserve(24);

            // clang-format off
            constexpr std::array<std::array<PosType, 3>, 24> positions = {{
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
            constexpr std::array<std::array<NormType, 3>, 6> normals = {{ { 0, 0, 1 },  { 0, 0, -1 },
                                                                         { -1, 0, 0 }, { 1, 0, 0 },
                                                                         { 0, 1, 0 }, { 0, -1, 0 }, }};

            if constexpr (HasUVs) {
                constexpr std::array<std::array<UVType, 2>, 24> uvs = {{
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
                    // Setting these individually is required because they
                    // could be opaque types, so memcpy won't work, and
                    // the types cannot be guaranteed to be trivially
                    // castable to each other, so assignment won't work.
                    vert.position[0] = positions[i][0];
                    vert.position[1] = positions[i][1];
                    vert.position[2] = positions[i][2];
                    vert.normal[0] = normals[i / 4][0];
                    vert.normal[1] = normals[i / 4][1];
                    vert.normal[2] = normals[i / 4][2];
                    vert.uv[0] = uvs[i][0];
                    vert.uv[1] = uvs[i][1];
                    return_mesh->get_vertices().push_back(vert);
                }
            } else { // Does not have UV data
                for (size_t i = 0; i < 24; i++) {
                    T vert;
                    vert.position[0] = positions[i][0];
                    vert.position[1] = positions[i][1];
                    vert.position[2] = positions[i][2];
                    vert.normal[0] = normals[i / 6][0];
                    vert.normal[1] = normals[i / 6][1];
                    vert.normal[2] = normals[i / 6][2];
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
            // TODO: There should be a way to evaluate these loops at compile-time.
            for (PosType i = -1; i <= 1; i += 2) {
                for (PosType j = -1; j <= 1; j += 2) {
                    for (PosType k = -1; k <= 1; k += 2) {
                        T vert;
                        vert.position = { i, j, k };
                        return_mesh->get_vertices().push_back(vert);
                    }
                }
            }

            // clang-format off
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
        }

        if constexpr (ColorComponentsCount > 0) {
            for (auto& vert : return_mesh->get_vertices()) {
                set_color(vert);
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
        if constexpr (ColorComponentsCount > 0) {
            set_color(vert_one);
            set_color(vert_two);
            set_color(vert_three);
        }
        if constexpr (HasNormals) {
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
        vert_one.position = { -1, 1, 0 };
        T vert_two;
        vert_two.position = { 1, 1, 0 };
        T vert_three;
        vert_three.position = { -1, -1, 0 };
        T vert_four;
        vert_four.position = { 1, -1, 0 };
        if constexpr (ColorComponentsCount > 0) {
            set_color(vert_one);
            set_color(vert_two);
            set_color(vert_three);
            set_color(vert_four);
        }
        if constexpr (HasNormals) {
            vert_one.normal = { 1, 1, 0 };
            vert_two.normal = { -1, 1, 0 };
            vert_three.normal = { -1, -1, 0 };
            vert_four.normal = { 1, -1, 0 };
        }
        return_mesh->get_vertices().push_back(vert_one);
        return_mesh->get_vertices().push_back(vert_two);
        return_mesh->get_vertices().push_back(vert_three);
        return_mesh->get_vertices().push_back(vert_four);
        // clang-format off
        return_mesh->get_indices() = {
            0, 1, 2,
            2, 1, 3,
        };
        // clang-format on
        break;
    }

    case mesh_type::none:
    default:
        return nullptr;
    }

    if (!return_mesh->create(device)) {
        return nullptr;
    }
    return return_mesh;
}

} // namespace lava
