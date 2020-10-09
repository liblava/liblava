// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/asset/raw_image.hpp>

namespace lava {

    raw_image::raw_image(std::vector<unsigned char> pixels, uv2 size, ui32 channels)
        : m_pixels(pixels), m_size(size), m_channels(channels)
    {
    }

    std::vector<unsigned char> raw_image::pixels() const
    {
        return m_pixels;
    }

    uv2 raw_image::size() const
    {
        return m_size;
    }

    ui32 raw_image::channels() const
    {
        return m_channels;
    }

} // namespace lava
