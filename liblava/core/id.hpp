// file      : liblava/core/id.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <atomic>
#include <deque>
#include <liblava/core/types.hpp>
#include <memory>
#include <mutex>
#include <set>

namespace lava {

    struct id {
        using ref = id const&;
        using set = std::set<id>;
        using set_ref = set const&;
        using list = std::vector<id>;
        using map = std::map<id, id>;
        using index_map = std::map<id, index>;
        using string_map = std::map<id, string>;

        type value = undef;
        ui32 version = 0;

        bool valid() const {
            return value != undef;
        }
        string to_string(bool show_version = false) const {
            char result[32];
            if (show_version)
                snprintf(result, sizeof(result), "%u.%u", value, version);
            else
                snprintf(result, sizeof(result), "%u", value);
            return string(result);
        }

        void invalidate() {
            *this = {};
        }

        bool operator==(ref rhs) const {
            return (value == rhs.value) && (version == rhs.version);
        }
        bool operator!=(ref rhs) const {
            return !(*this == rhs);
        }
        bool operator<(ref rhs) const {
            return std::tie(value, version) < std::tie(rhs.value, rhs.version);
        }

        bool check(id::map& map) {
            if (!valid())
                return false;

            if (!map.count(*this))
                return false;

            *this = map.at(*this);
            return true;
        }
    };

    using string_id_map = std::map<string, id>;
    constexpr id const undef_id = id();

    struct ids {
        static ids& global() {
            static ids instance;
            return instance;
        }

        static id next() {
            return ids::global().get_next();
        }
        static void free(id::ref id) {
            ids::global().reuse(id);
        }

        id get_next() {
            if (!reuse_ids)
                return { ++next_id };

            return get_next_locked();
        }

        void reuse(id::ref id) {
            if (reuse_ids)
                reuse_locked(id);
        }

        void set_reuse(bool state) {
            reuse_ids = state;
        }
        bool reusing() const {
            return reuse_ids;
        }

        type get_max() const {
            return next_id;
        }
        void set_max(type max) {
            if (max > next_id)
                next_id = max;
        }

    private:
        id get_next_locked() {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (free_ids.empty())
                return { ++next_id };

            auto next_id = free_ids.front();
            free_ids.pop_front();
            return { next_id.value, next_id.version + 1 };
        }

        void reuse_locked(id::ref id) {
            std::unique_lock<std::mutex> lock(queue_mutex);
            free_ids.push_back(id);
        }

        std::atomic<type> next_id = { undef };
        std::mutex queue_mutex;

        bool reuse_ids = true;
        std::deque<id> free_ids;
    };

    template<typename T>
    inline id add_id_map(T const& object, std::map<id, T>& map) {
        auto next = ids::next();
        map.emplace(next, std::move(object));
        return next;
    }

    template<typename T>
    inline bool remove_id_map(id::ref object, std::map<id, T>& map) {
        if (!map.count(object))
            return false;

        map.erase(object);
        ids::free(object);

        return true;
    }

    template<typename T>
    struct id_listeners {
        id add(typename T::func const& listener) {
            return add_id_map(listener, list);
        }

        void remove(id& id) {
            if (remove_id_map(id, list))
                id.invalidate();
        }

        typename T::listeners const& get_list() const {
            return list;
        }

    private:
        typename T::listeners list = {};
    };

    struct id_obj : interface {
        id_obj()
        : obj_id(ids::next()) {}
        ~id_obj() {
            ids::free(obj_id);
        }

        id::ref get_id() const {
            return obj_id;
        }

    private:
        id obj_id;
    };

    template<typename T, typename Meta>
    struct id_registry {
        using ptr = std::shared_ptr<T>;
        using map = std::map<id, ptr>;

        using meta_map = std::map<id, Meta>;

        id create(Meta info = {}) {
            auto object = std::make_shared<T>();
            add(object, info);

            return object->get_id();
        }

        void add(ptr object, Meta info = {}) {
            objects.emplace(object->get_id(), object);
            meta.emplace(object->get_id(), info);
        }

        bool has(id::ref object) const {
            return objects.count(object);
        }

        ptr get(id::ref object) const {
            return objects.at(object).get();
        }
        Meta get_meta(id::ref object) const {
            return meta.at(object).get();
        }

        map const& get_all() const {
            return objects;
        }
        meta_map const& get_all_meta() const {
            return meta;
        }

        void remove(id::ref object) {
            objects.erase(object);
        }

    private:
        map objects;
        meta_map meta;
    };

} // namespace lava
