/**
 * @file         liblava/file/file_utils.cpp
 * @brief        File utilities
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/core/misc.hpp"
#include "liblava/file/file.hpp"
#include "liblava/file/file_system.hpp"
#include "liblava/file/file_utils.hpp"

namespace lava {

//-----------------------------------------------------------------------------
bool read_file(std::vector<char>& out,
               string_ref filename) {
    std::ifstream file(str(filename),
                       std::ios::ate | std::ios::binary);
    LAVA_ASSERT(file.is_open());
    if (!file.is_open())
        return false;

    out.clear();

    auto file_size = to_size_t(file.tellg());
    if (file_size > 0) {
        out.resize(file_size);
        file.seekg(0, std::ios::beg);
        file.read(out.data(), file_size);
    }

    file.close();
    return true;
}

//-----------------------------------------------------------------------------
bool write_file(string_ref filename,
                char const* data,
                size_t data_size) {
    std::ofstream file(str(filename), std::ofstream::binary);
    LAVA_ASSERT(file.is_open());

    if (!file.is_open())
        return false;

    file.write(data, data_size);
    file.close();
    return true;
}

//-----------------------------------------------------------------------------
bool extension(string_ref filename, string_ref extension) {
    string to_check = filename.substr(filename.find_last_of('.') + 1);
    std::transform(to_check.begin(), to_check.end(), to_check.begin(), ::tolower);

    auto ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return to_check == ext;
}

//-----------------------------------------------------------------------------
bool extension(string_ref filename, string_list_ref extensions) {
    for (auto& ex : extensions)
        if (extension(filename, ex))
            return true;

    return false;
}

//-----------------------------------------------------------------------------
string get_filename_from(string_ref path, bool with_extension) {
    std::filesystem::path target(path);
    return with_extension
               ? target.filename().string()
               : target.stem().string();
}

//-----------------------------------------------------------------------------
bool remove_existing_path(string& target, string_ref path) {
    auto pos = target.find(path);
    if (pos != std::string::npos) {
        target.erase(pos, path.length());

#ifdef _WIN32
        std::replace(target.begin(), target.end(), '\\', '/');
#endif

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
bool load_file_data(string_ref filename, data& target) {
    file file(filename);
    if (!file.opened())
        return false;

    target.set(to_size_t(file.get_size()));
    if (!target.ptr)
        return false;

    return !file_error(file.read(target.ptr));
}

//-----------------------------------------------------------------------------
file_delete::~file_delete() {
    if (active)
        std::filesystem::remove(filename);
}

} // namespace lava