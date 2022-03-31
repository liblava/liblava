/**
 * @file         liblava/asset/mesh_loader.hpp
 * @brief        Load mesh from file
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/resource/mesh.hpp>

namespace lava {

/**
 * @brief Load mesh from file
 *
 * @param device        Vulkan device
 * @param filename      File to load
 *
 * @return mesh::ptr    Loaded mesh
 */
mesh::ptr load_mesh(device_ptr device, string_ref filename);

} // namespace lava
