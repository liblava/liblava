// file      : liblava/resource/mesh.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/resource/buffer.hpp>
#include <liblava/core/math.hpp>
#include <liblava/core/data.hpp>

namespace lava {

struct vertex {

    using list = std::vector<vertex>;

    v3 position;
    v3 color;
    v2 uv;
    v3 normal;

    bool operator==(vertex const& other) const {

        return position == other.position && color == other.color && uv == other.uv && normal == other.normal;
    }
};

struct mesh_data {

    vertex::list vertices;
    index_list indices;

    void move(v3 position) {

        for (auto& vertex : vertices)
            vertex.position += position;
    }

    void scale(r32 factor) {

        for (auto& vertex : vertices)
            vertex.position *= factor;
    }
};

struct mesh {

    using ptr = std::shared_ptr<mesh>;
    using map = std::map<id, ptr>;
    using list = std::vector<ptr>;

    static ptr make() { return std::make_shared<mesh>(); }

    explicit mesh();
    ~mesh();

    bool create(device* device, bool mapped = false, VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU);
    void destroy();

    void bind(VkCommandBuffer cmd_buffer) const;
    void draw(VkCommandBuffer cmd_buffer) const;

    id::ref get_id() const { return _id; }
    device* get_device() { return dev; }

    bool empty() const { return data.vertices.empty(); }

    void set_data(mesh_data const& value) { data = value; }
    mesh_data& get_data() { return data; }
    void add_data(mesh_data const& value);

    vertex::list& get_vertices() { return data.vertices; }
    vertex::list const& get_vertices() const { return data.vertices; }
    ui32 get_vertices_count() const { return to_ui32(data.vertices.size()); }

    index_list& get_indices() { return data.indices; }
    index_list const& get_indices() const { return data.indices; }
    ui32 get_indices_count() const { return to_ui32(data.indices.size()); }

    bool reload();

    buffer::ptr get_vertex_buffer() { return vertex_buffer; }
    buffer::ptr get_index_buffer() { return index_buffer; }

private:
    id _id;
    device* dev = nullptr;

    mesh_data data;

    buffer::ptr vertex_buffer;
    buffer::ptr index_buffer;

    bool mapped = false;
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
};

mesh::ptr load_mesh(device* device, name filename);

BETTER_ENUM(mesh_type, type, none = 0, cube, triangle, quad)

mesh::ptr load_mesh(device* device, mesh_type type);

struct mesh_meta {

    string filename; // empty -> type
    mesh_type type = mesh_type::none;
};

using mesh_registry = registry<mesh, mesh_meta>;

} // lava
