// file      : liblava/resource/meshloader.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/asset/mesh.hpp>

namespace lava {

    mesh::ptr load_mesh(device_ptr device, name filename);

} // namespace lava
