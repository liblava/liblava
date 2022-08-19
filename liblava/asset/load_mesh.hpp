/**
 * @file         liblava/asset/load_mesh.hpp
 * @brief        Load mesh from file
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/resource/mesh.hpp>

namespace lava {

/**
 * @brief Load mesh from file
 * @param device        Vulkan device
 * @param filename      File to load
 * @param temp_dir      Temporary directory
 * @return mesh::ptr    Loaded mesh
 */
mesh::ptr load_mesh(device_p device,
                    string_ref filename,
                    string_ref temp_dir);

} // namespace lava
