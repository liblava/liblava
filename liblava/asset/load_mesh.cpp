/**
 * @file         liblava/asset/load_mesh.cpp
 * @brief        Load mesh from file
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/asset/load_mesh.hpp>
#include <liblava/file.hpp>

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

namespace lava {

//-----------------------------------------------------------------------------
mesh::ptr load_mesh(device_p device, string_ref filename, string_ref temp_dir) {
    if (extension(filename, "OBJ")) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;
        std::string warn;

        auto target_file = filename;

        file_delete temp_file_delete;
        {
            file file(filename);
            if (file.opened() && file.get_type() == file_type::fs) {
                string temp_file;
                temp_file = temp_dir;
                temp_file += get_filename_from(target_file, true);

                unique_data temp_data(file.get_size());
                if (!temp_data.ptr)
                    return nullptr;

                if (file_error(file.read(temp_data.ptr)))
                    return nullptr;

                if (!write_file(temp_file, temp_data.ptr, temp_data.size))
                    return nullptr;

                target_file = temp_file;

                temp_file_delete.filename = target_file;
            }
        }

        if (tinyobj::LoadObj(&attrib,
                             &shapes,
                             &materials,
                             &warn, &err,
                             str(target_file))) {
            auto mesh = make_mesh();

            for (auto const& shape : shapes) {
                for (auto const& index : shape.mesh.indices) {
                    vertex vertex;

                    vertex.position = v3(attrib.vertices[3 * index.vertex_index],
                                         attrib.vertices[3 * index.vertex_index + 1],
                                         attrib.vertices[3 * index.vertex_index + 2]);

                    vertex.color = v4(1.f);

                    if (!attrib.texcoords.empty())
                        vertex.uv = v2(attrib.texcoords[2 * index.texcoord_index],
                                       1.f - attrib.texcoords[2 * index.texcoord_index + 1]);

                    vertex.normal = attrib.normals.empty()
                                        ? v3(0.f)
                                        : v3(attrib.normals[3 * index.normal_index],
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

    return nullptr;
}

} // namespace lava
