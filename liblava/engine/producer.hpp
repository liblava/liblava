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
     * @param mesh_type     Type of mesh
     * @return mesh::ptr    Mesh
     */
    mesh::ptr create_mesh(mesh_type mesh_type);

    /**
     * @brief Get mesh by prop name
     * @param name          Name of prop
     * @return mesh::ptr    Mesh
     */
    mesh::ptr get_mesh(string_ref name);

    /**
     * @brief Add mesh to products
     * @param mesh      Mesh
     * @return true     Added to products
     * @return false    Already exists
     */
    bool add_mesh(mesh::ptr mesh);

    /**
     * @brief Create a texture product
     * @param size             Size of texture
     * @return texture::ptr    Default texture
     */
    texture::ptr create_texture(uv2 size);

    /**
     * @brief Get texture by prop name
     * @param name             Name of prop
     * @return texture::ptr    Texture
     */
    texture::ptr get_texture(string_ref name);

    /**
     * @brief Add texture to products
     * @param product    Texture
     * @return true      Added to products
     * @return false     Already exists
     */
    bool add_texture(texture::ptr product);

    /**
     * @brief Generate shader by prop name
     * @param name      Name of shader
     * @param reload    Reload shader
     * @return cdata    Shader data
     */
    cdata get_shader(string_ref name,
                     bool reload = false);

    /**
     * @brief Regenerate shader by prop name
     * @param name      Name of shader
     * @return cdata    Shader data
     */
    cdata reload_shader(string_ref name) {
        return get_shader(name, true);
    }

    /**
     * @brief Compile shader
     * @param product         Shader data
     * @param name            Shader name
     * @param filename        Shader filename
     * @return data           Compiled shader data
     */
    data compile_shader(cdata product,
                        string_ref name,
                        string_ref filename) const;

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

    /**
     * @brief Shader optimization level
     */
    enum shader_optimization : type {
        none = 0,
        size,
        performance
    };

    /// Shader optimization level
    shader_optimization shader_opt = shader_optimization::performance;

    /**
     * @brief Shader source language
     */
    enum shader_language : type {
        glsl = 0,
        hlsl
    };

    /// Shader source language
    shader_language shader_lang = shader_language::glsl;

    /// Shader debug information
    bool shader_debug = false;

private:
    /**
     * @brief Update file hash
     * @param name             Target file
     * @param file_hash_map    Map of used files with hash
     */
    void update_file_hash(string_ref name,
                          string_map_ref file_hash_map) const;

    /**
     * @brief Check if shader file(s) changed
     * @param name      Name of shader
     * @return true     Shader is valid
     * @return false    Shader has changed
     */
    bool valid_shader(string_ref name) const;

    /// Map of shader products
    using shader_map = std::map<string, data>;

    /// Shader products
    shader_map shaders;
};

/// Producer type
using producer_t = producer;

} // namespace lava
