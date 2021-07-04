/**
 * @file         liblava/resource/generic_mesh.hpp
 * @brief        Generic vulkan mesh
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

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

    /**
     * @brief Move mesh data by offset
     *
     * @tparam PosVecType Type of coordinate vector
     * @tparam PosType    Type of coordinate element
     *
     * @param offset      Position offset
     * @param factor      Byte offset of coordinate vector within T
     */
    template<typename PosVecType = lava::v3, typename PosType = float>
    void move(std::array<PosType, 3> offset, size_t struct_pos_offset = 0) {
        for (T& vertex : vertices) {
            for (size_t i = 0; i < 3; i++) {
                (*((std::array<PosType, 3>*) (&vertex + struct_pos_offset)))[i] += offset[i];
            }
        }
    }

    /**
     * @brief Scale mesh data by factor
     *
     * @tparam PosType  Type of coordinate element
     *
     * @param factor    Position scaling factor
     * @param factor    Byte offset of coordinate vector within T
     */
    template<typename PosType = float>
    void scale(PosType factor, size_t struct_pos_offset = 0) {
        for (T& vertex : vertices) {
            for (size_t i = 0; i < 3; i++) {
                (*((std::array<PosType, 3>*) (&vertex + struct_pos_offset)))[i] *= factor;
            }
        }
    }

private:
    static constexpr bool const is_default = std::is_same_v<T, vertex>;
};

/**
 * @brief Temporary templated mesh
 *
 * @tparam T Input vertex struct
 */
template<typename T = vertex>
struct generic_mesh : id_obj {
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

template<typename T>
inline std::shared_ptr<generic_mesh<T>> generic_make_mesh() {
    return std::make_shared<generic_mesh<T>>();
}

//-----------------------------------------------------------------------------
template<typename T = vertex, typename PosType>
std::shared_ptr<generic_mesh<T>> generic_create_mesh(device_ptr device, mesh_type type);

template<typename T>
bool generic_mesh<T>::create(device_ptr d, bool m, VmaMemoryUsage mu) {
    device = d;
    mapped = m;
    memory_usage = mu;

    if (!data.vertices.empty()) {
        vertex_buffer = make_buffer();

        if (!vertex_buffer->create(device, data.vertices.data(), sizeof(vertex) * data.vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, mapped, memory_usage)) {
            log()->error("create mesh vertex buffer");
            return false;
        }
    }

    if (!data.indices.empty()) {
        index_buffer = make_buffer();

        if (!index_buffer->create(device, data.indices.data(), sizeof(ui32) * data.indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, mapped, memory_usage)) {
            log()->error("create mesh index buffer");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
template<typename T = lava::vertex, typename PosType = float>
std::shared_ptr<generic_mesh<T>> generic_create_mesh(device_ptr device, mesh_type type, size_t pos_offset = offsetof(T, position)) {
    std::shared_ptr<generic_mesh<T>> triangle = generic_make_mesh<T>();
    std::array<PosType, 3> pos_one = { 1, 1, 0 };
    T vert_one;
    memcpy(&vert_one + pos_offset, &pos_one, sizeof(pos_one));
    std::array<PosType, 3> pos_two = { -1, 1, 0 };
    T vert_two;
    memcpy(&vert_two + pos_offset, &pos_two, sizeof(pos_two));
    std::array<PosType, 3> pos_three = { 0, -1, 0 };
    T vert_three;
    memcpy(&vert_three + pos_offset, &pos_three, sizeof(pos_three));

    triangle->get_vertices().push_back(vert_one);
    triangle->get_vertices().push_back(vert_two);
    triangle->get_vertices().push_back(vert_three);
    if (!triangle->create(device))
        return nullptr;
    return triangle;
}

}
