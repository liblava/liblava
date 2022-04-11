/**
 * @file         liblava/engine/producer.hpp
 * @brief        Producer
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/fwd.hpp>
#include <liblava/resource.hpp>

namespace lava {

/**
 * @brief Producer
 */
struct producer {
    /// Engine
    engine* context = nullptr;

    /**
     * @brief Create a mesh product
     *
     * @param mesh_type     Type of mesh
     *
     * @return mesh::ptr    Mesh
     */
    mesh::ptr create_mesh(mesh_type mesh_type);

    /**
     * @brief Get mesh by prop name
     *
     * @param name          Name of prop
     *
     * @return mesh::ptr    Mesh
     */
    mesh::ptr get_mesh(string_ref name);

    /**
     * @brief Add mesh to products
     *
     * @param mesh      Mesh
     *
     * @return true     Added to products
     * @return false    Already exists
     */
    bool add_mesh(mesh::ptr mesh);

    /**
     * @brief Create a texture product
     *
     * @param size             Size of texture
     *
     * @return texture::ptr    Default texture
     */
    texture::ptr create_texture(uv2 size);

    /**
     * @brief Get texture by prop name
     *
     * @param name             Name of prop
     *
     * @return texture::ptr    Texture
     */
    texture::ptr get_texture(string_ref name);

    /**
     * @brief Add texture to products
     *
     * @param product    Texture
     *
     * @return true      Added to products
     * @return false     Already exists
     */
    bool add_texture(texture::ptr product);

    /**
     * @brief Generate shader by prop name
     *
     * @param name      Name of shader
     *
     * @return cdata    Shader data
     */
    cdata get_shader(string_ref name);

    /**
     * @brief Destroy all products
     */
    void destroy();

    /**
     * @brief Clear all products
     */
    void clear();

    /// Mesh products
    id_registry<mesh, string> meshes;

    /// Texture products
    id_registry<texture, string> textures;

private:
    /// Map of shader products
    using shader_map = std::map<string, data>;

    /// Shader products
    shader_map shaders;
};

} // namespace lava
