// file      : liblava/resource/mesh.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/resource/mesh.hpp>

#ifndef LIBLAVA_TINYOBJLOADER
#define LIBLAVA_TINYOBJLOADER 1
#endif

#if LIBLAVA_TINYOBJLOADER

#ifdef _WIN32
#pragma warning(push, 4)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

#endif

namespace lava {

void mesh::add_data(mesh_data const& value) {

    auto index_base = to_ui32(data.vertices.size());

    data.vertices.insert(data.vertices.end(), value.vertices.begin(),value.vertices.end());

    for (auto& index : value.indices)
        data.indices.push_back(index_base + index);
}

bool mesh::create(device* device, bool mapped_, VmaMemoryUsage memory_usage_) {

    dev = device;
    mapped = mapped_;
    memory_usage = memory_usage_;

    if (!data.vertices.empty()) {

        vertex_buffer = buffer::make();

        if (!vertex_buffer->create(device, data.vertices.data(), sizeof(vertex) * data.vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, mapped, memory_usage)) {

            log()->error("mesh::create vertexBuffer create failed");
            return false;
        }
    }

    if (!data.indices.empty()) {

        index_buffer = buffer::make();

        if (!index_buffer->create(device, data.indices.data(), sizeof(ui32) * data.indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, mapped, memory_usage)) {

            log()->error("mesh::create indexBuffer create failed");
            return false;
        }
    }

	return true;
}

void mesh::destroy() {

    vertex_buffer = nullptr;
    index_buffer = nullptr;

    dev = nullptr;
}

bool mesh::reload() {

    auto device = dev;
    destroy();

    return create(device, mapped, memory_usage);
}

void mesh::bind(VkCommandBuffer cmd_buf) const {

    if (vertex_buffer && vertex_buffer->is_valid()) {

        std::array<VkDeviceSize, 1> const buffer_offsets = { 0 };
        std::array<VkBuffer, 1> const buffers = { vertex_buffer->get() };

        vkCmdBindVertexBuffers(cmd_buf, 0, to_ui32(buffers.size()), buffers.data(), buffer_offsets.data());
    }

    if (index_buffer && index_buffer->is_valid())
        vkCmdBindIndexBuffer(cmd_buf, index_buffer->get(), 0, VK_INDEX_TYPE_UINT32);
}

void mesh::draw(VkCommandBuffer cmd_buf) const {

    if (!data.indices.empty())
        vkCmdDrawIndexed(cmd_buf, to_ui32(data.indices.size()), 1, 0, 0, 0);
    else
        vkCmdDraw(cmd_buf, to_ui32(data.vertices.size()), 1, 0, 0);
}

} // lava

lava::mesh::ptr lava::load_mesh(device* device, name filename) {

#if LIBLAVA_TINYOBJLOADER
    if (has_extension(filename, "OBJ")) {

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;
        std::string warn;

        string target_file = filename;

        file_guard temp_file_remover;
        {
            file file(filename);
            if (file.is_open() && file.get_type() == file_type::fs) {

                string temp_file;
                temp_file = file_system::get_pref_dir();
                temp_file += get_filename_from(target_file, true);

                scope_data temp_data(file.get_size());
                if (!temp_data.ptr)
                    return nullptr;

                if (is_file_error(file.read(temp_data.ptr)))
                    return nullptr;

                if (!write_file(temp_file.c_str(), temp_data.ptr, temp_data.size))
                    return nullptr;

                target_file = temp_file;

                temp_file_remover.filename = target_file;
            }
        }

        if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, target_file.c_str())) {

            auto mesh = mesh::make();

            for (auto const& shape : shapes) {

                for (auto const& index : shape.mesh.indices) {

                    vertex vertex;

                    vertex.position = v3(attrib.vertices[3 * index.vertex_index],
                                         attrib.vertices[3 * index.vertex_index + 1],
                                         attrib.vertices[3 * index.vertex_index + 2]);

                    vertex.color = v4(1.f);

                    if (!attrib.texcoords.empty())
                        vertex.uv = v2(attrib.texcoords[2 * index.texcoord_index], 1.f - attrib.texcoords[2 * index.texcoord_index + 1]);

                    vertex.normal = attrib.normals.empty() ? v3(0.f) : v3(attrib.normals[3 * index.normal_index], 
                                                                          attrib.normals[3 * index.normal_index + 1], 
                                                                          attrib.normals[3 * index.normal_index + 2]);

                    mesh->get_vertices().push_back(vertex);
                    mesh->get_indices().push_back(mesh->get_indices_count());
                }
            }

            if (mesh->empty())
                return nullptr;

            if (!mesh->create(device))
                return nullptr;

            return mesh;
        }
    }
#endif

    return nullptr;
}

lava::mesh::ptr lava::load_mesh(device* device, mesh_type type) {

    switch (type) {

    case mesh_type::cube: {

        auto cube = mesh::make();
        cube->get_vertices() = {

            // front
            { {  1.f,  1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 0.f, 0.f, 1.f } },
            { { -1.f,  1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 0.f, 0.f, 1.f } },
            { { -1.f, -1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 0.f, 0.f, 1.f } },
            { {  1.f, -1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 0.f, 0.f, 1.f } },

            // back
            { {  1.f,  1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 0.f, 0.f, -1.f } },
            { { -1.f,  1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 0.f, 0.f, -1.f } },
            { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 0.f, 0.f, -1.f } },
            { {  1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 0.f, 0.f, -1.f } },

            // left
            { { -1.f,  1.f,  1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { -1.f, 0.f, 0.f } },
            { { -1.f,  1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { -1.f, 0.f, 0.f } },
            { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { -1.f, 0.f, 0.f } },
            { { -1.f, -1.f,  1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { -1.f, 0.f, 0.f } },

            // right
            { { 1.f,  1.f,  1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 1.f, 0.f, 0.f } },
            { { 1.f, -1.f,  1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 1.f, 0.f, 0.f } },
            { { 1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 1.f, 0.f, 0.f } },
            { { 1.f,  1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 1.f, 0.f, 0.f } },

            // bottom
            { {  1.f, 1.f,  1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 0.f, 1.f, 0.f } },
            { { -1.f, 1.f,  1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 0.f, 1.f, 0.f } },
            { { -1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 0.f, 1.f, 0.f } },
            { {  1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 0.f, 1.f, 0.f } },

            // top
            { {  1.f, -1.f,  1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 0.f, -1.f, 0.f } },
            { { -1.f, -1.f,  1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 0.f, -1.f, 0.f } },
            { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 0.f, -1.f, 0.f } },
            { {  1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 0.f, -1.f, 0.f } },
        };

        cube->get_indices() = { 0, 1, 2,
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

        if (!cube->create(device))
            return nullptr;

        return cube;
    }

    case mesh_type::triangle: {

        auto triangle = mesh::make();

        triangle->get_vertices().push_back({ {  1.f,  1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f,  1.f }, { 0.f, 0.f, 1.f } });
        triangle->get_vertices().push_back({ { -1.f,  1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f,  1.f }, { 0.f, 0.f, 1.f } });
        triangle->get_vertices().push_back({ {  0.f, -1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.5f, 0.f }, { 0.f, 0.f, 1.f } });

        if (!triangle->create(device))
            return nullptr;

        return triangle;
    }

    case mesh_type::quad: {

        auto quad = mesh::make();

        quad->get_vertices() = 
        {
            { {  1.f,  1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 0.f, 0.f, 1.f } },
            { { -1.f,  1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 0.f, 0.f, 1.f } },
            { { -1.f, -1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 0.f, 0.f, 1.f } },
            { {  1.f, -1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 0.f, 0.f, 1.f } },
        };

        quad->get_indices() = { 0, 1, 2, 2, 3, 0 };

        if (!quad->create(device))
            return nullptr;

        return quad;
    }

    case mesh_type::none:
    default:
        return nullptr;
    }
}
