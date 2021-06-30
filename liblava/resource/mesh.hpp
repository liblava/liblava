/**
 * @file         liblava/resource/mesh.hpp
 * @brief        Vulkan mesh
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/resource/buffer.hpp>

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
template<typename T = vertex>
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
    void set_data(mesh_data const& value) {
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

#include "mesh.tpp"
