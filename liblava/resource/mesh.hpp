/**
 * @file         liblava/resource/mesh.hpp
 * @brief        Vulkan mesh
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/resource/buffer.hpp>
// #include <liblava/resource/generic_mesh.hpp>

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

    /// Vertex UV
    v2 uv;

    /// Vertex Normal
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
 * @brief Mesh data
 */
struct mesh_data {
    /// List of vertices
    vertex::list vertices;

    /// List of indices
    index_list indices;

    /**
     * @brief Move mesh data by offset
     * 
     * @param offset    Position offset
     */
    void move(v3 offset) {
        for (auto& vertex : vertices)
            vertex.position += offset;
    }

    /**
     * @brief Scale mesh data by factor
     * 
     * @param factor    Position factor
     */
    void scale(r32 factor) {
        for (auto& vertex : vertices)
            vertex.position *= factor;
    }
};

/**
 * @brief Temporary templated mesh data
 *
 * @tparam T Input vertex struct
 */
template<typename T = lava::vertex>
struct generic_mesh_data {
    /// List of vertices
    std::vector<T> vertices;

    /// List of indices.
    index_list indices;

    void move(v3 offset) {
        static_assert(is_lava,
                      "This method may only be called when lava::vertex is the mesh_data's input struct.");
        if constexpr (is_lava) {
            for (auto& vertex : vertices)
                vertex.position += offset;
        }
    }

    void scale(r32 factor) {
        static_assert(is_lava,
                      "This method may only be called when lava::vertex is the mesh_data's input struct.");
        if constexpr (is_lava) {
            for (auto& vertex : vertices)
                vertex.position *= factor;
        }
    }

private:
    static constexpr bool const is_lava = std::is_same_v<T, lava::vertex>;
};

/**
 * @brief Temporary templated mesh
 *
 * @tparam T Input vertex struct
 */
template<typename T = lava::vertex>
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
    mesh_data& get_data() {
        return data;
    }
    bool reload();

private:
    device_ptr device = nullptr;
    mesh_data data;
    buffer::ptr vertex_buffer;
    buffer::ptr index_buffer;
    bool mapped = false;
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
};

//-----------------------------------------------------------------------------
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

/**
 * @brief Mesh
 */
struct mesh : id_obj {
    /// Shared pointer to mesh
    using ptr = std::shared_ptr<mesh>;

    /// Map of meshes
    using map = std::map<id, ptr>;

    /// List of meshes
    using list = std::vector<ptr>;

    /**
     * @brief Destroy the mesh
     */
    ~mesh() {
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
    bool create(device_ptr device, bool mapped = false, VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU);

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
     * @brief Get the device
     * 
     * @return device_ptr    Vulkan device
     */
    device_ptr get_device() {
        return device;
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
    void set_data(mesh_data const& value) {
        data = value;
    }

    /**
     * @brief Get the mesh data
     * 
     * @return mesh_data&    Mesh data
     */
    mesh_data& get_data() {
        return data;
    }

    /**
     * @brief Add mesh data to existing data
     * 
     * @param value    Mesh data to add
     */
    void add_data(mesh_data const& value);

    /**
     * @brief Get the vertices of the mesh
     * 
     * @return vertex::list&    List of vertices
     */
    vertex::list& get_vertices() {
        return data.vertices;
    }

    /**
     * @brief Get the const vertices of the mesh
     * 
     * @return vertex::list const&    List of vertices
     */
    vertex::list const& get_vertices() const {
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
    mesh_data data;

    /// Vertex buffer
    buffer::ptr vertex_buffer;

    /// Index buffer
    buffer::ptr index_buffer;

    /// Mapped state
    bool mapped = false;

    /// Memory usage
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
};

/**
 * @brief Make a new mesh
 * 
 * @return mesh::ptr    Shared pointer to mesh
 */
inline mesh::ptr make_mesh() {
    return std::make_shared<mesh>();
}

/**
 * @brief Mesh types
 */
enum class mesh_type : type {
    none = 0,
    cube,
    triangle,
    quad
};

/**
 * @brief Create a new mesh
 * 
 * @param device        Vulkan device
 * @param type          Mesh type
 * 
 * @return mesh::ptr    Shared pointer to mesh
 */
mesh::ptr create_mesh(device_ptr device, mesh_type type);

//-----------------------------------------------------------------------------
template<typename T = lava::vertex, typename PosType>
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
template<typename T, typename PosType>
std::shared_ptr<generic_mesh<T>> generic_create_mesh(device_ptr device, mesh_type type, size_t pos_offset) {
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
