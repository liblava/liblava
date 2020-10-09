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
        raw_image(std::vector<unsigned char> pixels = std::vector<unsigned char>(), uv2 size = {0, 0}, ui32 channels = 0);

        std::vector<unsigned char> pixels() const;

        uv2 size() const;

        ui32 channels() const;

    private:
        std::vector<unsigned char> m_pixels;
        uv2 m_size = uv2(0, 0);
        ui32 m_channels = 0;
    };

} // namespace lava
