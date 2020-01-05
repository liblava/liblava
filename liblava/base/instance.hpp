// file      : liblava/base/instance.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/physical_device.hpp>

namespace lava {

struct instance : no_copy_no_move {

    struct create_param {

        names layer_to_enable;
        names extension_to_enable;

        bool check() const;
    };

    struct debug_config {

        bool validation = false;
        bool assistent = false;
        bool render_doc = false;
        bool verbose = false;
        bool utils = false;
        bool info = false;
    };

    static instance& singleton() {

        static instance instance;
        return instance;
    }

    bool create(create_param& param, debug_config& debug, name app_name = nullptr);
    void destroy();

    static VkLayerPropertiesList enumerate_layer_properties();
    static VkExtensionPropertiesList enumerate_extension_properties(name layer_name = nullptr);

    physical_device::list const& get_physical_devices() const { return physical_devices; }

    static physical_device const& get_first_physical_device() { return singleton().physical_devices.front(); }

    static VkInstance get() { return singleton().vk_instance; }

    static internal_version get_version();

private:
    explicit instance() = default;
    ~instance();

    bool enumerate_physical_devices();

    void print_info() const;

    bool create_validation_report();
    void destroy_validation_report();

    VkInstance vk_instance = nullptr;

    physical_device::list physical_devices;

    debug_config debug;
    VkDebugUtilsMessengerEXT debug_messanger = 0;
};

} // lava
