/**
 * @file         liblava/engine/producer.hpp
 * @brief        Producer
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/fwd.hpp"
#include "liblava/resource.hpp"

namespace lava {

/// shader folder
constexpr name _shader_path_ = "shader/";

/// temp folder
constexpr name _temp_path_ = "temp/";

/// hash file
constexpr name _hash_json_ = "hash.json";

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
     * @return Added to products or already exists
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
     * @return Added to products or already exists
     */
    bool add_texture(texture::ptr product);

    /**
     * @brief Generate shader by prop name
     * @param name       Name of shader
     * @param reload     Reload shader
     * @return c_data    Shader data
     */
    c_data get_shader(string_ref name,
                      bool reload = false);

    /**
     * @brief Regenerate shader by prop name
     * @param name       Name of shader
     * @return c_data    Shader data
     */
    c_data reload_shader(string_ref name) {
        return get_shader(name, true);
    }

    /**
     * @brief Compile shader
     * @param product         Shader data
     * @param name            Shader name
     * @param filename        Shader filename
     * @return data           Compiled shader data
     */
    data compile_shader(c_data product,
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
    enum shader_optimization : index {
        none = 0,
        size,
        performance
    };

    /// Shader optimization level
    shader_optimization shader_opt = shader_optimization::performance;

    /**
     * @brief Shader source language
     */
    enum shader_language : index {
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
    void update_hash(string_ref name,
                     string_map_ref file_hash_map) const;

    /**
     * @brief Check if shader file(s) changed
     * @param name      Name of shader
     * @return Shader is valid or has changed
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
