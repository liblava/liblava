// file      : liblava/file/file_utils.cpp
// authors   : Lava Block OÜ and contributors
// copyright : Copyright (c) 2018-present, MIT License

#include <liblava/file/file.hpp>
#include <liblava/file/file_system.hpp>
#include <liblava/file/file_utils.hpp>
#include <liblava/util/log.hpp>

namespace lava {

//-----------------------------------------------------------------------------
bool read_file(std::vector<char>& out, name filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    assert(file.is_open());

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
bool write_file(name filename, char const* data, size_t data_size) {
    std::ofstream file(filename, std::ofstream::binary);
    assert(file.is_open());

    if (!file.is_open())
        return false;

    file.write(data, data_size);
    file.close();
    return true;
}

//-----------------------------------------------------------------------------
bool extension(name filename, name extension) {
    string fn = filename;
    string ext = extension;

    string to_check = fn.substr(fn.find_last_of('.') + 1);

    std::transform(to_check.begin(), to_check.end(), to_check.begin(), ::tolower);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return to_check == ext;
}

//-----------------------------------------------------------------------------
bool extension(name filename, names extensions) {
    for (auto& ex : extensions)
        if (extension(filename, ex))
            return true;

    return false;
}

//-----------------------------------------------------------------------------
string get_filename_from(string_ref path, bool with_extension) {
    fs::path target(path);
    return with_extension ? target.filename().string() : target.stem().string();
}

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
    file file(str(filename));
    if (!file.opened())
        return false;

    target.set(to_size_t(file.get_size()));
    if (!target.ptr)
        return false;

    if (file_error(file.read(target.ptr))) {
        log()->error("read file {}", filename);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
file_remover::~file_remover() {
    if (remove)
        fs::remove(filename);
}

} // namespace lava