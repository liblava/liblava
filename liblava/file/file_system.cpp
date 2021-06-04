// file      : liblava/file/file_system.cpp
// authors   : Lava Block OÜ and contributors
// copyright : Copyright (c) 2018-present, MIT License

#include <physfs.h>
#include <liblava/file/file_system.hpp>
#include <liblava/util/log.hpp>

namespace lava {

//-----------------------------------------------------------------------------
internal_version file_system::get_version() {
    PHYSFS_Version result;
    PHYSFS_getLinkedVersion(&result);

    return { result.major, result.minor, result.patch };
}

//-----------------------------------------------------------------------------
name file_system::get_base_dir() {
    return PHYSFS_getBaseDir();
}

//-----------------------------------------------------------------------------
string file_system::get_base_dir_str() {
    return string(get_base_dir());
}

//-----------------------------------------------------------------------------
name file_system::get_pref_dir() {
    return PHYSFS_getPrefDir(instance().org, instance().app);
}

//-----------------------------------------------------------------------------
string file_system::get_res_dir_str() {
    string res_dir = get_base_dir();
    res_dir += instance().res_path;
    string_ref const_res_dir = res_dir;

    return fs::path(const_res_dir).lexically_normal().string();
}

//-----------------------------------------------------------------------------
bool file_system::mount(string_ref path) {
    return PHYSFS_mount(str(path), nullptr, 1) != 0;
}

//-----------------------------------------------------------------------------
bool file_system::mount(name base_dir_path) {
    return mount(get_base_dir_str() + base_dir_path);
}

//-----------------------------------------------------------------------------
bool file_system::exists(name file) {
    return PHYSFS_exists(file) != 0;
}

//-----------------------------------------------------------------------------
name file_system::get_real_dir(name file) {
    return PHYSFS_getRealDir(file);
}

//-----------------------------------------------------------------------------
string_list file_system::enumerate_files(name path) {
    string_list result;

    auto rc = PHYSFS_enumerateFiles(path);
    for (auto i = rc; *i != nullptr; ++i)
        result.push_back(*i);

    PHYSFS_freeList(rc);

    return result;
}

//-----------------------------------------------------------------------------
bool file_system::initialize(name argv_0, name o, name a, name e) {
    assert(!initialized); // only once
    if (initialized)
        return initialized;

    if (!initialized) {
        PHYSFS_init(argv_0);

        PHYSFS_setSaneConfig(o, a, e, 0, 0);
        initialized = true;

        org = o;
        app = a;
        ext = e;
    }

    return initialized;
}

//-----------------------------------------------------------------------------
void file_system::terminate() {
    if (!initialized)
        return;

    PHYSFS_deinit();
}

//-----------------------------------------------------------------------------
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

    if (fs::exists(str(get_res_dir_str())))
        if (file_system::mount(str(res_path)))
            log()->debug("mount {}", str(get_res_dir_str()));

    auto cwd_res_dir = fs::current_path().append("res/").lexically_normal().string();

    if (fs::exists(cwd_res_dir) && (cwd_res_dir != get_res_dir_str()))
        if (file_system::mount(cwd_res_dir))
            log()->debug("mount {}", str(cwd_res_dir));

    string archive_file = "res.zip";
    if (fs::exists({ archive_file }))
        if (file_system::mount(str(archive_file)))
            log()->debug("mount {}", str(archive_file));
}

//-----------------------------------------------------------------------------
bool file_system::create_data_folder() {
    fs::path data_path = fs::current_path();
    data_path += fs::path::preferred_separator;
    data_path += "data";

    if (!fs::exists(data_path))
        fs::create_directories(data_path);

    return fs::exists(data_path);
}

//-----------------------------------------------------------------------------
void file_system::clean_pref_dir() {
    fs::remove_all(get_pref_dir());
}

} // namespace lava
