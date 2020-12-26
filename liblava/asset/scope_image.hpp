// file      : liblava/asset/scope_image.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/math.hpp>
#include <liblava/file/file.hpp>

namespace lava {

    struct scope_image {
        explicit scope_image(string_ref filename);
        ~scope_image();

        bool ready = false;

        data_ptr data = nullptr;
        uv2 size = uv2(0, 0);
        ui32 channels = 0;

    private:
        file image_file;
        scope_data file_data;
    };

} // namespace lava
