/**
 * @file         liblava/engine/producer.cpp
 * @brief        Producer
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/asset.hpp>
#include <liblava/engine/engine.hpp>
#include <liblava/engine/producer.hpp>

namespace lava {

//-----------------------------------------------------------------------------
mesh::ptr producer::create_mesh(mesh_type mesh_type) {
    auto product = lava::create_mesh(context->device, mesh_type);
    if (!add_mesh(product))
        return nullptr;

    return product;
}

//-----------------------------------------------------------------------------
mesh::ptr producer::get_mesh(string_ref name) {
    for (auto& [id, meta] : meshes.get_all_meta()) {
        if (meta == name)
            return meshes.get(id);
    }

    auto product = load_mesh(context->device, context->prop.get_filename(name));
    if (!product)
        return nullptr;

    if (!add_mesh(product))
        return nullptr;

    return product;
}

//-----------------------------------------------------------------------------
bool producer::add_mesh(mesh::ptr product) {
    if (!product)
        return false;

    if (meshes.exists(product->get_id()))
        return false;

    meshes.add(product);
    return true;
}

//-----------------------------------------------------------------------------
texture::ptr producer::create_texture(uv2 size) {
    auto product = create_default_texture(context->device, size);
    if (!add_texture(product))
        return nullptr;

    return product;
}

//-----------------------------------------------------------------------------
texture::ptr producer::get_texture(string_ref name) {
    for (auto& [id, meta] : textures.get_all_meta()) {
        if (meta == name)
            return textures.get(id);
    }

    auto product = load_texture(context->device, context->prop.get_filename(name));
    if (!product)
        return nullptr;

    if (!add_texture(product))
        return nullptr;

    return product;
}

//-----------------------------------------------------------------------------
bool producer::add_texture(texture::ptr product) {
    if (!product)
        return false;

    if (textures.exists(product->get_id()))
        return false;

    textures.add(product);

    context->staging.add(product);
    return true;
}

//-----------------------------------------------------------------------------
void producer::destroy() {
    for (auto& [id, mesh] : meshes.get_all())
        mesh->destroy();

    for (auto& [id, texture] : textures.get_all())
        texture->destroy();
}

} // namespace lava
