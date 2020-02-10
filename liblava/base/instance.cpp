// file      : liblava/base/instance.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/base/instance.hpp>
#include <liblava/base/memory.hpp>

#define VK_LAYER_LUNARG_STANDARD_VALIDATION_NAME "VK_LAYER_LUNARG_standard_validation"
#define VK_LAYER_LUNARG_ASSISTENT_LAYER_NAME "VK_LAYER_LUNARG_assistant_layer"
#define VK_LAYER_RENDERDOC_CAPTURE_NAME "VK_LAYER_RENDERDOC_Capture"

namespace lava {

instance::~instance() {

    destroy();
}

bool instance::create(create_param& param, debug_config& debug_, name app_name) {

    debug = debug_;

    if (debug.validation) {

        if (!exists(param.layer_to_enable, VK_LAYER_LUNARG_STANDARD_VALIDATION_NAME))
            param.layer_to_enable.push_back(VK_LAYER_LUNARG_STANDARD_VALIDATION_NAME);
    }

    if (debug.render_doc) {

        if (!exists(param.layer_to_enable, VK_LAYER_RENDERDOC_CAPTURE_NAME))
            param.layer_to_enable.push_back(VK_LAYER_RENDERDOC_CAPTURE_NAME);
    }

    if (debug.assistent) {

        if (!exists(param.layer_to_enable, VK_LAYER_LUNARG_ASSISTENT_LAYER_NAME))
            param.layer_to_enable.push_back(VK_LAYER_LUNARG_ASSISTENT_LAYER_NAME);
    }

    if (debug.utils) {

        if (!exists(param.extension_to_enable, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            param.extension_to_enable.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    if (!param.check()) {

        log()->error("create instance param");
        return false;
    }

    VkApplicationInfo application_info
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = app_name ? app_name : _lava_,
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .pEngineName = _lava_,
    };

    application_info.engineVersion = VK_MAKE_VERSION(_internal_version.major, _internal_version.minor, _internal_version.patch);
    application_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = to_ui32(param.layer_to_enable.size()),
        .ppEnabledLayerNames = param.layer_to_enable.data(),
        .enabledExtensionCount = to_ui32(param.extension_to_enable.size()),
        .ppEnabledExtensionNames = param.extension_to_enable.data(),
    };

    auto result = vkCreateInstance(&instanceCreateInfo, memory::alloc(), &vk_instance);
    if (failed(result))
        return false;

    volkLoadInstance(vk_instance);

    if (!enumerate_physical_devices())
        return false;

    if (debug.info)
        print_info();

    if (debug.utils)
        if (!create_validation_report())
            return false;

    return true;
}

void instance::destroy() {

    if (!vk_instance)
        return

    physical_devices.clear();

    if (debug.utils)
        destroy_validation_report();

    vkDestroyInstance(vk_instance, memory::alloc());
    vk_instance = nullptr;
}

bool instance::create_param::check() const {

    auto layer_properties = enumerate_layer_properties();
    for (auto const& layer_name : layer_to_enable) {

        auto itr = std::find_if(layer_properties.begin(), layer_properties.end(), 
                            [&](VkLayerProperties const& extProp) {

                                return strcmp(layer_name, extProp.layerName) == 0;
                            });

        if (itr == layer_properties.end())
            return false;
    }

    auto extensions_properties = enumerate_extension_properties();
    for (auto const& ext_name : extension_to_enable) {

        auto itr = std::find_if(extensions_properties.begin(), extensions_properties.end(),
                            [&](VkExtensionProperties const& extProp) {

                                return strcmp(ext_name, extProp.extensionName) == 0;
                            });

        if (itr == extensions_properties.end())
            return false;
    }

    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL validation_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {

    if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {

        log()->error("validation report: {}", callback_data->pMessage);
        assert(!"check validation error");
    }
    else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        log()->warn("validation report: {}", callback_data->pMessage);
    else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        log()->info("validation report: {}", callback_data->pMessage);
    else if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        log()->trace("validation report: {}", callback_data->pMessage);

    return VK_FALSE;
}

bool instance::create_validation_report() {

    VkDebugUtilsMessengerCreateInfoEXT create_info
    {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = validation_callback,
    };

    if (debug.verbose)
        create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    return check(vkCreateDebugUtilsMessengerEXT(vk_instance, &create_info, memory::alloc(), &debug_messanger));
}

void instance::destroy_validation_report() {

    if (!debug_messanger)
        return;

    vkDestroyDebugUtilsMessengerEXT(vk_instance, debug_messanger, memory::alloc());

    debug_messanger = 0;
}

void instance::print_info() const {

    auto global_extensions = instance::enumerate_extension_properties();
    log()->info("{} global extensions", global_extensions.size());
    for (auto const& extension : global_extensions) {

        log()->info("- {:38} - {:3}", extension.extensionName, VK_VERSION_PATCH(extension.specVersion));
    }

    auto layer_properties = instance::enumerate_layer_properties();
    log()->info("{} instance layers:", layer_properties.size());
    for (auto const& layer : layer_properties) {

        log()->info("- {:40} - {:8} - {:2} - {}", layer.layerName, version_to_string(layer.specVersion), layer.implementationVersion, layer.description);

        auto extensions = instance::enumerate_extension_properties(layer.layerName);
        log()->info("-- {} extensions", extensions.size());

        for (auto const& extension : extensions) {

            log()->info("--- {:38} - {:6}", extension.extensionName, version_to_string(extension.specVersion));
        }
    }

    log()->info("{} physical devices", physical_devices.size());

    auto device_index = 0u;
    for (auto& physical_device : physical_devices) {

        auto& properties = physical_device.get_properties();

        log()->info("- {}. {} - {}", device_index++, properties.deviceName, physical_device.get_device_type_string());
        log()->info("-- apiVersion: {} - driverVersion: {}", version_to_string(properties.apiVersion), version_to_string(properties.driverVersion));
        log()->info("-- vendorID: {} - deviceID: {}", properties.vendorID, properties.deviceID);
    }
}

VkLayerPropertiesList instance::enumerate_layer_properties() {

    auto layer_count = 0u;
    auto result = vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    if (failed(result))
        return {};

    VkLayerPropertiesList list(layer_count);
    result = vkEnumerateInstanceLayerProperties(&layer_count, list.data());
    if (failed(result))
        return {};

    return list;
}

VkExtensionPropertiesList instance::enumerate_extension_properties(name layer_name) {

    auto property_count = 0u;
    auto result = vkEnumerateInstanceExtensionProperties(layer_name, &property_count, nullptr);
    if (failed(result))
        return {};

    VkExtensionPropertiesList list(property_count);
    result = vkEnumerateInstanceExtensionProperties(layer_name, &property_count, list.data());
    if (failed(result))
        return {};

    return list;
}

bool instance::enumerate_physical_devices() {

    physical_devices.clear();

    auto count = 0u;
    auto result = vkEnumeratePhysicalDevices(vk_instance, &count, nullptr);
    if (failed(result))
        return false;

    VkPhysicalDevices devices(count);
    result = vkEnumeratePhysicalDevices(vk_instance, &count, devices.data());
    if (failed(result))
        return false;

    for (auto& device : devices) {

        physical_device physical_device;
        physical_device.initialize(device);
        physical_devices.push_back(std::move(physical_device));
    }

    return true;
}

internal_version instance::get_version() {

    ui32 instance_version = VK_API_VERSION_1_0;

    auto enumerate_instance_version = (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion");
    if (enumerate_instance_version)
        enumerate_instance_version(&instance_version);

    internal_version version;
    version.major = VK_VERSION_MAJOR(instance_version);
    version.minor = VK_VERSION_MINOR(instance_version);
    version.patch = VK_VERSION_PATCH(VK_HEADER_VERSION);
    return version;
}

} // lava
