/*
#include <liblava/resource/mesh.hpp>

namespace lava {
//-----------------------------------------------------------------------------
template<typename T>
void generic_mesh<T>::add_data(lava::generic_mesh_data<T> const& value) {
    auto index_base = to_ui32(data.vertices.size());

    data.vertices.insert(data.vertices.end(), value.vertices.begin(), value.vertices.end());

    for (auto& index : value.indices)
        data.indices.push_back(index_base + index);
}

//-----------------------------------------------------------------------------
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
template<typename T>
void generic_mesh<T>::destroy() {
    vertex_buffer = nullptr;
    index_buffer = nullptr;

    device = nullptr;
}

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
bool generic_mesh<T>::reload() {
    auto dev = device;
    destroy();

    return create(dev, mapped, memory_usage);
}

//-----------------------------------------------------------------------------
template<typename T, typename PosType>
generic_mesh<T> generic_create_mesh(device_ptr device, mesh_type type) {
    auto triangle = generic_mesh<T>::generic_make_mesh();
    std::array<PosType, 3> pos_one = { 1, 1, 0 };
    std::array<PosType, 3> pos_two = { -1, 1, 0 };
    std::array<PosType, 3> pos_three = { 0, -1, 0 };
    // triangle->get_vertices().push_back(pos_one);
    // triangle->get_vertices().push_back(pos_two);
    // triangle->get_vertices().push_back(pos_three);
    return triangle;
}
}; // namespace lava
*/
