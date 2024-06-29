/**
 * @file         liblava/file/file.cpp
 * @brief        File access
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/file/file.hpp"
#include "physfs.h"

namespace lava {

//-----------------------------------------------------------------------------
file::file(string_ref p, file_mode m) {
    open(p, m);
}

//-----------------------------------------------------------------------------
file::~file() {
    close();
}

//-----------------------------------------------------------------------------
bool file::open(string_ref p, file_mode m) {
    if (p.empty())
        return false;

    m_path = p;
    m_mode = m;

    if (m_mode == file_mode::write)
        m_file = PHYSFS_openWrite(str(m_path));
    else
        m_file = PHYSFS_openRead(str(m_path));

    if (m_file) {
        m_type = file_type::fs;
    } else {
        if (m_mode == file_mode::write) {
            m_ostream = std::ofstream(m_path,
                                      std::ios::binary);
            if (m_ostream.is_open())
                m_type = file_type::f_stream;
        } else {
            m_istream = std::ifstream(m_path,
                                      std::ios::binary);
            if (m_istream.is_open())
                m_type = file_type::f_stream;

            m_istream.seekg(0, m_istream.beg);
        }
    }

    return opened();
}

//-----------------------------------------------------------------------------
void file::close() {
    if (m_type == file_type::fs) {
        PHYSFS_close(m_file);
    } else if (m_type == file_type::f_stream) {
        if (m_mode == file_mode::write)
            m_ostream.close();
        else
            m_istream.close();
    }
}

//-----------------------------------------------------------------------------
bool file::opened() const {
    if (m_type == file_type::fs) {
        return m_file != nullptr;
    } else if (m_type == file_type::f_stream) {
        if (m_mode == file_mode::write)
            return m_ostream.is_open();
        else
            return m_istream.is_open();
    }

    return false;
}

//-----------------------------------------------------------------------------
i64 file::get_size() const {
    if (m_type == file_type::fs) {
        return PHYSFS_fileLength(m_file);
    } else if (m_type == file_type::f_stream) {
        if (m_mode == file_mode::write) {
            auto current = m_ostream.tellp();
            m_ostream.seekp(0, m_ostream.end);
            auto result = m_ostream.tellp();
            m_ostream.seekp(current);
            return result;
        } else {
            auto current = m_istream.tellg();
            m_istream.seekg(0, m_istream.end);
            auto result = m_istream.tellg();
            m_istream.seekg(current);
            return result;
        }
    }

    return file_error_result;
}

//-----------------------------------------------------------------------------
i64 file::read(data::ptr data, ui64 size) {
    if (m_mode == file_mode::write)
        return file_error_result;

    if (m_type == file_type::fs)
        return PHYSFS_readBytes(m_file, data, size);
    else if (m_type == file_type::f_stream)
        return m_istream.read(data, size).gcount();

    return file_error_result;
}

//-----------------------------------------------------------------------------
i64 file::write(data::c_ptr data, ui64 size) {
    if (m_mode != file_mode::write)
        return file_error_result;

    if (m_type == file_type::fs) {
        return PHYSFS_writeBytes(m_file, data, size);
    } else if (m_type == file_type::f_stream) {
        m_ostream.write(data, size);
        return to_i64(size);
    }

    return file_error_result;
}

//-----------------------------------------------------------------------------
i64 file::seek(ui64 position) {
    if (m_type == file_type::fs) {
        return PHYSFS_seek(m_file, position);
    } else if (m_type == file_type::f_stream) {
        if (m_mode == file_mode::write)
            m_ostream.seekp(position);
        else
            m_istream.seekg(position);

        return tell();
    }

    return file_error_result;
}

//-----------------------------------------------------------------------------
i64 file::tell() const {
    if (m_type == file_type::fs) {
        return PHYSFS_tell(m_file);
    } else if (m_type == file_type::f_stream) {
        if (m_mode == file_mode::write)
            return to_i64(m_ostream.tellp());
        else
            return to_i64(m_istream.tellg());
    }

    return file_error_result;
}

} // namespace lava
