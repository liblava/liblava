// file      : liblava/asset/mesh.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/asset/mesh.hpp>

namespace lava {

    void mesh::add_data(mesh_data const& value) {
        auto index_base = to_ui32(data.vertices.size());

        data.vertices.insert(data.vertices.end(), value.vertices.begin(), value.vertices.end());

        for (auto& index : value.indices)
            data.indices.push_back(index_base + index);
    }

    bool mesh::create(device_ptr d, bool m, VmaMemoryUsage mu) {
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

    void mesh::destroy() {
        vertex_buffer = nullptr;
        index_buffer = nullptr;

        device = nullptr;
    }

    bool mesh::reload() {
        auto dev = device;
        destroy();

        return create(dev, mapped, memory_usage);
    }

    void mesh::bind(VkCommandBuffer cmd_buf) const {
        if (vertex_buffer && vertex_buffer->valid()) {
            std::array<VkDeviceSize, 1> const buffer_offsets = { 0 };
            std::array<VkBuffer, 1> const buffers = { vertex_buffer->get() };

            vkCmdBindVertexBuffers(cmd_buf, 0, to_ui32(buffers.size()), buffers.data(), buffer_offsets.data());
        }

        if (index_buffer && index_buffer->valid())
            vkCmdBindIndexBuffer(cmd_buf, index_buffer->get(), 0, VK_INDEX_TYPE_UINT32);
    }

    void mesh::draw(VkCommandBuffer cmd_buf) const {
        if (!data.indices.empty())
            vkCmdDrawIndexed(cmd_buf, to_ui32(data.indices.size()), 1, 0, 0, 0);
        else
            vkCmdDraw(cmd_buf, to_ui32(data.vertices.size()), 1, 0, 0);
    }

} // namespace lava

lava::mesh::ptr lava::generate_mesh(device_ptr device, mesh_type type) {
    switch (type) {
    case mesh_type::cube: {
        auto cube = make_mesh();
        cube->get_vertices() = {
            // front
            { { 1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 0.f, 0.f, 1.f } },
            { { -1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 0.f, 0.f, 1.f } },
            { { -1.f, -1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 0.f, 0.f, 1.f } },
            { { 1.f, -1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 0.f, 0.f, 1.f } },

            // back
            { { 1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 0.f, 0.f, -1.f } },
            { { -1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 0.f, 0.f, -1.f } },
            { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 0.f, 0.f, -1.f } },
            { { 1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 0.f, 0.f, -1.f } },

            // left
            { { -1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { -1.f, 0.f, 0.f } },
            { { -1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { -1.f, 0.f, 0.f } },
            { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { -1.f, 0.f, 0.f } },
            { { -1.f, -1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { -1.f, 0.f, 0.f } },

            // right
            { { 1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 1.f, 0.f, 0.f } },
            { { 1.f, -1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 1.f, 0.f, 0.f } },
            { { 1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 1.f, 0.f, 0.f } },
            { { 1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 1.f, 0.f, 0.f } },

            // bottom
            { { 1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 0.f, 1.f, 0.f } },
            { { -1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 0.f, 1.f, 0.f } },
            { { -1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 0.f, 1.f, 0.f } },
            { { 1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 0.f, 1.f, 0.f } },

            // top
            { { 1.f, -1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 0.f, -1.f, 0.f } },
            { { -1.f, -1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 0.f, -1.f, 0.f } },
            { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 0.f, -1.f, 0.f } },
            { { 1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 0.f, -1.f, 0.f } },
        };

        cube->get_indices() = {
            0,
            1,
            2,
            2,
            3,
            0,
            4,
            7,
            6,
            6,
            5,
            4,
            8,
            9,
            10,
            10,
            11,
            8,
            12,
            13,
            14,
            14,
            15,
            12,
            16,
            19,
            18,
            18,
            17,
            16,
            20,
            21,
            22,
            22,
            23,
            20,
        };

        if (!cube->create(device))
            return nullptr;

        return cube;
    }

    case mesh_type::triangle: {
        auto triangle = make_mesh();

        triangle->get_vertices().push_back({ { 1.f, 1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 0.f, 0.f, 1.f } });
        triangle->get_vertices().push_back({ { -1.f, 1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 0.f, 0.f, 1.f } });
        triangle->get_vertices().push_back({ { 0.f, -1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.5f, 0.f }, { 0.f, 0.f, 1.f } });

        if (!triangle->create(device))
            return nullptr;

        return triangle;
    }

    case mesh_type::quad: {
        auto quad = make_mesh();

        quad->get_vertices() = {
            { { 1.f, 1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f }, { 0.f, 0.f, 1.f } },
            { { -1.f, 1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 1.f }, { 0.f, 0.f, 1.f } },
            { { -1.f, -1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f }, { 0.f, 0.f, 1.f } },
            { { 1.f, -1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 0.f }, { 0.f, 0.f, 1.f } },
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
