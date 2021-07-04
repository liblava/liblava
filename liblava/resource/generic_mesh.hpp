#pragma once

#include "vulkan/vulkan_core.h"

namespace lava {

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
