// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/asset/mesh.hpp>

namespace lava {

    // Load a mesh file from disk. Will return a valid mesh pointer if successful, or a nullptr if loading failed.
    mesh::ptr load_mesh(device_ptr device, string_ref filename);

} // namespace lava
