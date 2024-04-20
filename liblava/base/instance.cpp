/**
 * @file         liblava/base/instance.cpp
 * @brief        Vulkan instance
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/base/instance.hpp"
#include "liblava/base/memory.hpp"
#include "liblava/core/misc.hpp"
#include "liblava/util/log.hpp"

#define VK_LAYER_KHRONOS_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"
#define VK_LAYER_RENDERDOC_CAPTURE_NAME "VK_LAYER_RENDERDOC_Capture"

namespace lava {

/**
 * @see https://khronos.org/registry/vulkan/specs/1.3-extensions/man/html/PFN_vkDebugUtilsMessengerCallbackEXT.html
 */
VKAPI_ATTR VkBool32 VKAPI_CALL
debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                         VkDebugUtilsMessageTypeFlagsEXT message_type,
                         const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                         void* user_data) {
    string type;
    if (message_type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
        type = "general";
    else if (message_type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
        type = "validation";
    else if (message_type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        type = "performance";
    else if (message_type & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT)
        type = "device address binding";

    auto log_msg = callback_data->pMessageIdName != nullptr
                       ? fmt::format("debug utils ({}): {} ({})",
                                     type,
                                     callback_data->pMessageIdName,
                                     callback_data->messageIdNumber)
                       : fmt::format("debug utils ({}): ({})",
                                     type,
                                     callback_data->messageIdNumber);

    string message(callback_data->pMessage);
    trim(message);

    log_msg += " - " + message;

    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        logger()->trace(log_msg);
    else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        logger()->trace(log_msg);
    else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        logger()->warn(log_msg);
    else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        logger()->error(log_msg);

        auto const skip_assert = callback_data->pMessageIdName
                                 && (string(callback_data->pMessageIdName) == "VUID-VkSwapchainCreateInfoKHR-imageExtent-01274");
        // unpreventable error application is still ok, see: https://github.com/KhronosGroup/Vulkan-ValidationLayers/issues/1340

        if (!skip_assert)
            LAVA_ASSERT(!"check debug utils error");
    }

    return VK_FALSE;
}

//-----------------------------------------------------------------------------
instance::~instance() {
    destroy();
}

//-----------------------------------------------------------------------------
bool instance::check_debug(create_param& param) const {
    if (debug.validation) {
        if (!exists(param.layers, VK_LAYER_KHRONOS_VALIDATION_NAME))
            param.layers.push_back(VK_LAYER_KHRONOS_VALIDATION_NAME);
    }

    if (debug.render_doc) {
        if (!exists(param.layers, VK_LAYER_RENDERDOC_CAPTURE_NAME))
            param.layers.push_back(VK_LAYER_RENDERDOC_CAPTURE_NAME);
    }

    if (debug.utils) {
        if (!exists(param.extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            param.extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    if (!check(param)) {
        logger()->error("create instance param");

        for (auto& extension : param.extensions)
            logger()->debug("extension: {}", extension);

        for (auto& layer : param.layers)
            logger()->debug("layer: {}", layer);

        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
bool instance::create(create_param& param,
                      debug_config::ref d,
                      instance_info::ref i) {
    debug = d;
    info = i;

    if (!check_debug(param))
        return false;

    VkApplicationInfo application_info{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = info.app_name,
        .applicationVersion = to_vk_version(info.app_version),
        .pEngineName = info.engine_name,
        .engineVersion = to_vk_version(info.engine_version),
    };

    switch (info.req_api_version) {
    case api_version::v1_1:
        application_info.apiVersion = VK_API_VERSION_1_1;
        break;
    case api_version::v1_2:
        application_info.apiVersion = VK_API_VERSION_1_2;
        break;
    case api_version::v1_3:
        application_info.apiVersion = VK_API_VERSION_1_3;
        break;
    default:
        application_info.apiVersion = VK_API_VERSION_1_0;
    }

    VkInstanceCreateInfo create_info{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = to_ui32(param.layers.size()),
        .ppEnabledLayerNames = param.layers.data(),
        .enabledExtensionCount = to_ui32(param.extensions.size()),
        .ppEnabledExtensionNames = param.extensions.data(),
#ifdef __APPLE__
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
    };

    if (failed(vkCreateInstance(&create_info,
                                memory::instance().alloc(),
                                &vk_instance)))
        return false;

    volkLoadInstance(vk_instance);

    if (!enumerate_physical_devices())
        return false;

    if (debug.utils)
        if (!create_debug_messenger())
            return false;

    return true;
}

//-----------------------------------------------------------------------------
void instance::destroy() {
    if (!vk_instance)
        return;

    physical_devices.clear();

    if (debug.utils)
        destroy_debug_messenger();

    vkDestroyInstance(vk_instance,
                      memory::instance().alloc());
    vk_instance = nullptr;
}

//-----------------------------------------------------------------------------
bool instance::create_debug_messenger() {
    VkDebugUtilsMessengerCreateInfoEXT create_info{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
        .pfnUserCallback = debug_messenger_callback,
    };

    if (debug.verbose)
        create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                       | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    return check(vkCreateDebugUtilsMessengerEXT(vk_instance,
                                                &create_info,
                                                memory::instance().alloc(),
                                                &debug_messenger));
}

//-----------------------------------------------------------------------------
void instance::destroy_debug_messenger() {
    if (!debug_messenger)
        return;

    vkDestroyDebugUtilsMessengerEXT(vk_instance,
                                    debug_messenger,
                                    memory::instance().alloc());

    debug_messenger = VK_NULL_HANDLE;
}

//-----------------------------------------------------------------------------
bool instance::enumerate_physical_devices() {
    physical_devices.clear();

    auto count = 0u;
    auto result = vkEnumeratePhysicalDevices(vk_instance,
                                             &count,
                                             nullptr);
    if (failed(result))
        return false;

    VkPhysicalDevices devices(count);
    result = vkEnumeratePhysicalDevices(vk_instance,
                                        &count,
                                        devices.data());
    if (failed(result))
        return false;

    for (auto& device : devices)
        physical_devices.emplace_back(physical_device::make(device));

    return true;
}

//-----------------------------------------------------------------------------
bool check(instance::create_param::ref param) {
    auto layer_properties = enumerate_layer_properties();
    for (auto const& layer_name : param.layers) {
        auto itr = std::find_if(layer_properties.begin(), layer_properties.end(),
                                [&](VkLayerProperties const& extProp) {
                                    return strcmp(layer_name, extProp.layerName) == 0;
                                });

        if (itr == layer_properties.end())
            return false;
    }

    auto extensions_properties = enumerate_extension_properties();
    for (auto const& ext_name : param.extensions) {
        auto itr = std::find_if(
            extensions_properties.begin(), extensions_properties.end(),
            [&](VkExtensionProperties const& extProp) {
                return strcmp(ext_name, extProp.extensionName) == 0;
            });

        if (itr == extensions_properties.end())
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
sem_version get_instance_version() {
    ui32 instance_version = VK_API_VERSION_1_0;

    auto enumerate_instance_version =
        (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(nullptr,
                                                              "vkEnumerateInstanceVersion");
    if (enumerate_instance_version)
        enumerate_instance_version(&instance_version);

    sem_version version;
    version.major = VK_VERSION_MAJOR(instance_version);
    version.minor = VK_VERSION_MINOR(instance_version);
    version.patch = VK_VERSION_PATCH(VK_HEADER_VERSION);
    return version;
}

//-----------------------------------------------------------------------------
VkLayerPropertiesList enumerate_layer_properties() {
    auto layer_count = 0u;
    auto result = vkEnumerateInstanceLayerProperties(&layer_count,
                                                     nullptr);
    if (failed(result))
        return {};

    VkLayerPropertiesList list(layer_count);
    result = vkEnumerateInstanceLayerProperties(&layer_count,
                                                list.data());
    if (failed(result))
        return {};

    return list;
}

//-----------------------------------------------------------------------------
VkExtensionPropertiesList enumerate_extension_properties(name layer_name) {
    auto property_count = 0u;
    auto result = vkEnumerateInstanceExtensionProperties(layer_name,
                                                         &property_count,
                                                         nullptr);
    if (failed(result))
        return {};

    VkExtensionPropertiesList list(property_count);
    result = vkEnumerateInstanceExtensionProperties(layer_name,
                                                    &property_count,
                                                    list.data());
    if (failed(result))
        return {};

    return list;
}

} // namespace lava
