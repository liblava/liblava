// file      : liblava/util/file.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/util/file.hpp>
#include <liblava/util/utility.hpp>

#include <physfs.h>

bool lava::read_file(std::vector<char>& out, name filename) {

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

bool lava::write_file(name filename, char const* data, size_t data_size) {

    std::ofstream file(filename, std::ofstream::binary);
    assert(file.is_open());

    if (!file.is_open())
        return false;

    file.write(data, data_size);
    file.close();
    return true;
}

bool lava::has_extension(name file_name, name extension) {

    string fn = file_name;
    string ext = extension;

    string to_check = fn.substr(fn.find_last_of('.') + 1);

    std::transform(to_check.begin(), to_check.end(), to_check.begin(), ::tolower);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return to_check == ext;
}

bool lava::has_extension(name filename, names extensions) {

    for (auto& extension : extensions)
        if (has_extension(filename, extension))
            return true;

    return false;
}

lava::string lava::get_filename_from(string_ref path, bool with_extension) {

    fs::path target(path);
    return with_extension ? target.filename().string() : target.stem().string();
}

bool lava::remove_existing_path(string& target, string_ref path) {

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

namespace lava {

internal_version file_system::get_version() {

    PHYSFS_Version result;
    PHYSFS_getLinkedVersion(&result);

    return { result.major, result.minor, result.patch };
}

name file_system::get_base_dir() { return PHYSFS_getBaseDir(); }

string file_system::get_base_dir_str() { return string(get_base_dir()); }

name file_system::get_pref_dir() { return PHYSFS_getPrefDir(get().org, get().app); }

string file_system::get_res_dir() {

    string res_dir = get_base_dir();
    res_dir += get().res_path;
    string_ref const_res_dir = res_dir;

    return fs::path(const_res_dir).lexically_normal().string();
}

bool file_system::mount(string_ref path) { return PHYSFS_mount(path.c_str(), nullptr, 1) != 0; }

bool file_system::mount(name base_dir_path) { return mount(get_base_dir_str() + base_dir_path); }

bool file_system::exists(name file) { return PHYSFS_exists(file) != 0; }

name file_system::get_real_dir(name file) { return PHYSFS_getRealDir(file); }

bool file_system::initialize(name argv_0, name org_, name app_, name ext_) {

    assert(!initialized); // only once
    if (initialized)
        return initialized;

    if (!initialized) {

        PHYSFS_init(argv_0);

        PHYSFS_setSaneConfig(org_, app_, ext_, 0, 0);
        initialized = true;
    }

    return initialized;
}

void file_system::terminate() { 

    if (!initialized)
        return;

    PHYSFS_deinit(); 
}

void file_system::mount_res() {

#if LIBLAVA_DEBUG
#if _WIN32
    res_path = "../../res/";
#else
    res_path = "../res/";
#endif
#else
    res_path = "res/";
#endif

    if (fs::exists(get_res_dir().c_str()))
        if (file_system::mount(res_path.c_str()))
            log()->debug("mounted {}", get_res_dir().c_str());

    string archive_file = "res.zip";
    if (fs::exists({ archive_file }))
        if (file_system::mount(archive_file.c_str()))
            log()->debug("mounted {}", archive_file.c_str());
}

bool file_system::create_data_folder() {

    fs::path data_path = fs::current_path();
    data_path += fs::path::preferred_separator;
    data_path += "data";

    if (!fs::exists(data_path))
        fs::create_directories(data_path);

    return fs::exists(data_path);
}

file::file(name path_, bool write) { open(path_, write); }

file::~file() { close(); }

bool file::open(name path_, bool write) {

    if (!path_)
        return false;

    path = path_;
    write_mode = write;

    if (write_mode)
        fs_file = PHYSFS_openWrite(path);
    else
        fs_file = PHYSFS_openRead(path);

    if (fs_file) {

        type = file_type::fs;

    } else {

        if (write) {

            o_stream = std::ofstream(path, std::ofstream::binary);
            if (o_stream.is_open())
                type = file_type::f_stream;

        } else {

            i_stream = std::ifstream(path, std::ios::binary | std::ios::ate);
            if (i_stream.is_open())
                type = file_type::f_stream;
        }
    }

    return is_open();
}

void file::close() {

    if (type == file_type::fs) {

        PHYSFS_close(fs_file);

    } else if (type == file_type::f_stream) {

        if (write_mode)
            o_stream.close();
        else
            i_stream.close();
    }
}

bool file::is_open() const {

    if (type == file_type::fs) {

        return fs_file != nullptr;

    } else if (type == file_type::f_stream) {

        if (write_mode)
            return o_stream.is_open();
        else
            return i_stream.is_open();
    }

    return false;
}

i64 file::get_size() const {

    if (type == file_type::fs) {

        return PHYSFS_fileLength(fs_file);

    } else if (type == file_type::f_stream) {

        if (write_mode)
            return to_i64(o_stream.tellp());
        else
            return to_i64(i_stream.tellg());
    }

    return file_error;
}

i64 file::read(data_ptr data, ui64 size) {

    if (write_mode)
        return file_error;

    if (type == file_type::fs) {

        return PHYSFS_readBytes(fs_file, data, size);

    } else if (type == file_type::f_stream) {

        i_stream.seekg(0, std::ios::beg);
        i_stream.read(data, size);
        return to_i64(size);
    }

    return file_error;
}

i64 file::write(data_cptr data, ui64 size) {

    if (!write_mode)
        return file_error;

    if (type == file_type::fs) {

        return PHYSFS_writeBytes(fs_file, data, size);

    } else if (type == file_type::f_stream) {

        o_stream.write(data, size);
        return to_i64(size);
    }

    return file_error;
}

config_file::config_file(name path) : path(path) {}

void config_file::add_callback(config_file_callback* callback) { callbacks.push_back(callback); }

void config_file::remove_callback(config_file_callback* callback) { remove(callbacks, callback); }

bool config_file::load() {

    scope_data data;
    if (!load_file_data(path, data)) {

        log()->error("couldn't load config file {}", path.c_str());
        return false;
    }

    auto j = json::parse({ data.ptr, data.size });

    for (auto callback : callbacks)
        callback->on_load(j);

    return true;
}

bool config_file::save() {

    file file(path.c_str(), true);
    if (!file.is_open()) {

        log()->error("couldn't save config file {}", path.c_str());
        return false;
    }

    json j;

    for (auto callback : callbacks)
        callback->on_save(j);

    auto jString = j.dump(4);

    file.write(jString.data(), jString.size());

    return true;
}

} // lava

bool lava::load_file_data(string_ref filename, scope_data& data) {

    file file(filename.c_str());
    if (!file.is_open()) {

        log()->error("couldn't open file {} for reading", filename);
        return false;
    }

    data.set(to_size_t(file.get_size()));
    if (!data.ptr)
        return false;

    if (is_file_error(file.read(data.ptr))) {

        log()->error("couldn't read file {}", filename);
        return false;
    }

    return true;
}
