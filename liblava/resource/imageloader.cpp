// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/resource/imageloader.hpp>
#include <liblava/util/file.hpp>

#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace lava {

    raw_image load_image(string_ref filepath)
    {
        // read data from file
        file image_file(str(filepath));
        scope_data file_data(image_file.get_size());
        if (image_file.opened())
        {
            if (!file_data.allocate() || file_error(image_file.read(file_data.ptr)))
            {
                return raw_image();
            }
        }
        // use stb_image to load file directly or load image from compressed file data
        i32 width, height, channels = 0;
        unsigned char * rawData = nullptr;
        if (image_file.opened())
        {
            rawData = stbi_load_from_memory((stbi_uc const*) file_data.ptr, to_i32(file_data.size), &width, &height, &channels, STBI_rgb_alpha);
        }
        else
        {
            rawData = stbi_load(str(filepath), &width, &height, &channels, STBI_rgb_alpha);
        }
        // check if loading worked
        if (rawData == nullptr || width <= 0 || height <= 0 || channels <= 0)
        {
            return raw_image();
        }
        // copy data to shared storage
        i32 dataSize = width * height * channels;
        std::vector<unsigned char> data(dataSize);
        std::memcpy(data.data(), rawData, dataSize);
        // free stb_image resources
        stbi_image_free(rawData);
        return raw_image(data, { width, height }, channels);
    }

}
