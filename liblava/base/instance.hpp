// file      : liblava/base/instance.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/physical_device.hpp>

namespace lava {

    struct instance_info {
        using ref = instance_info const&;

        name app_name = _lava_;
        name engine_name = _liblava_;

        internal_version app_version;
        internal_version engine_version;

        api_version req_api_version = api_version::v1_0;
    };

    struct instance : no_copy_no_move {
        struct create_param {
            using ref = create_param const&;

            names layers{};
            names extensions{};
        };

        struct debug_config {
            using ref = debug_config const&;

            bool validation = false;
            bool render_doc = false;
            bool verbose = false;
            bool utils = false;
        };

        static instance& singleton() {
            static instance instance;
            return instance;
        }

        bool create(create_param& param, debug_config::ref debug, instance_info::ref info);
        void destroy();

        static VkLayerPropertiesList enumerate_layer_properties();
        static VkExtensionPropertiesList enumerate_extension_properties(name layer_name = nullptr);

        physical_device::list const& get_physical_devices() const {
            return physical_devices;
        }

        static physical_device::ref get_first_physical_device() {
            return singleton().physical_devices.front();
        }

        static VkInstance get() {
            return singleton().vk_instance;
        }

        static internal_version get_version();

        debug_config::ref get_debug_config() const {
            return debug;
        }
        instance_info::ref get_info() const {
            return info;
        }

    private:
        explicit instance() = default;
        ~instance();

        bool check_debug(create_param& param) const;

        bool enumerate_physical_devices();

        bool create_validation_report();
        void destroy_validation_report();

        VkInstance vk_instance = nullptr;

        physical_device::list physical_devices;

        debug_config debug;
        instance_info info;

        VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    };

    bool check(instance::create_param::ref param);

} // namespace lava
