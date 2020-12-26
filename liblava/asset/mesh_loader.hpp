// file      : liblava/asset/mesh_loader.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/resource/mesh.hpp>

namespace lava {

    mesh::ptr load_mesh(device_ptr device, name filename);

} // namespace lava
