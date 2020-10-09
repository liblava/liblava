// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/asset/raw_image.hpp>

namespace lava {

    // Will try to load an image from a filepath using stb_image.
    // Will return raw image pixel data in raw_image.data() if successful,
    // else raw_image.data() will be empty. 
    raw_image load_image(string_ref filepath);

} // namespace lava
