// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/data.hpp>
#include <liblava/core/math.hpp>
#include <liblava/core/types.hpp>

#include <vector>

namespace lava {

    class raw_image
    {
    public:
        // Create a new image from pixel data a dimensions.
        raw_image(std::vector<unsigned char> pixels = std::vector<unsigned char>(), uv2 size = {0, 0}, ui32 channels = 0);

        // Return raw pixel data. Will be empty if image is not valid.
        std::vector<unsigned char> pixels() const;

        // Return width and height of pixel data. Will be {0, 0} if image is not valid.
        uv2 size() const;

        // Return depth pixel data [1, 2, 3, 4]. Will be 0 if image is not valid.
        ui32 channels() const;

        // Check and return if image is valid (pixel data, dimensions and depth > 0).
        bool valid() const;

    private:
        std::vector<unsigned char> m_pixels;
        uv2 m_size = uv2(0, 0);
        ui32 m_channels = 0;
    };

} // namespace lava
