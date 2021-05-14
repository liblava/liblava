// file      : liblava/file/file_utils.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/data.hpp>

namespace lava {

    bool read_file(std::vector<char>& out, name filename);

    bool write_file(name filename, char const* data, size_t data_size);

    bool extension(name filename, name extension);

    bool extension(name filename, names extensions);

    string get_filename_from(string_ref path, bool with_extension = false);

    bool remove_existing_path(string& target, string_ref path);

    bool load_file_data(string_ref filename, data& target);

    struct file_data : scope_data {
        explicit file_data(string_ref filename) {
            load_file_data(filename, *this);
        }
    };

    struct file_remover : no_copy_no_move {
        explicit file_remover(name filename = "")
        : filename(filename) {}
        explicit file_remover(string filename)
        : filename(filename) {}

        ~file_remover();

        string filename;
        bool remove = true;
    };

} // namespace lava
