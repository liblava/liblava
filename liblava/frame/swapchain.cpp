// file      : liblava/frame/swapchain.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/frame/swapchain.hpp>
#include <liblava/base/instance.hpp>

namespace lava {

bool swapchain::create(device_ptr device_, VkSurfaceKHR surface_, uv2 size_, bool v_sync_) {

    device = device_;
    surface = surface_;
    size = size_;
    v_sync = v_sync_;

    set_surface_format();

    return create_internal();
}

void swapchain::destroy() {

    device->wait_for_idle();

    destroy_backbuffer_views();
    destroy_internal();

    vkDestroySurfaceKHR(instance::get(), surface, memory::alloc());
    surface = nullptr;
}

bool swapchain::resize(uv2 new_size) {

    device->wait_for_idle();

    if (!backbuffers.empty()) {

        for (auto& callback : callbacks)
            callback->on_destroyed();

        destroy_backbuffer_views();
    }

    size = new_size;
    if (size.x == 0 || size.y == 0)
        return true;

    auto result = create_internal();
    assert(result);
    if (!result)
        return false;

    for (auto& callback : reverse(callbacks)) {

        result = callback->on_created();
        assert(result);
        if (!result)
            return false;
    }

    reload_request = false;
    return true;
}

void swapchain::set_surface_format() {

    auto count = 0u;
    check(vkGetPhysicalDeviceSurfaceFormatsKHR(device->get_vk_physical_device(), surface, &count, nullptr));

    std::vector<VkSurfaceFormatKHR> formats(count);
    check(vkGetPhysicalDeviceSurfaceFormatsKHR(device->get_vk_physical_device(), surface, &count, formats.data()));

    if (count == 1) {

        if (formats[0].format == VK_FORMAT_UNDEFINED) {

            format.format = VK_FORMAT_B8G8R8A8_UNORM;
            format.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

        } else {

            format = formats[0];
        }

    } else {

        VkFormat const requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, 
                                                        VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };

        VkColorSpaceKHR const requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

        auto requestedFound = false;
        for (auto& i : requestSurfaceImageFormat) {

            if (requestedFound)
                break;

            for (ui32 j = 0; j < count; j++) {

                if (formats[j].format == i &&
                    formats[j].colorSpace == requestSurfaceColorSpace) {

                    format = formats[j];
                    requestedFound = true;
                }
            }
        }

        if (!requestedFound)
            format = formats[0];
    }
}

VkPresentModeKHR swapchain::choose_present_mode(VkPresentModeKHRs const& present_modes) const {

    if (v_sync)
        return VK_PRESENT_MODE_FIFO_KHR;

    for (auto const& present_mode : present_modes)
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return present_mode;

    return VK_PRESENT_MODE_FIFO_KHR;
}

bool swapchain::create_internal() {

    auto present_mode_count = 0u;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(device->get_vk_physical_device(), surface, &present_mode_count, nullptr) != VK_SUCCESS || present_mode_count == 0) {

        log()->error("swapchain::create_internal vkGetPhysicalDeviceSurfacePresentModesKHR(1) failed");
        return false;
    }

    VkPresentModeKHRs present_modes(present_mode_count);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(device->get_vk_physical_device(), surface, &present_mode_count, present_modes.data()) != VK_SUCCESS) {

        log()->error("swapchain::create_internal vkGetPhysicalDeviceSurfacePresentModesKHR(2) failed");
        return false;
    }

    VkPresentModeKHR present_mode = choose_present_mode(present_modes);

    VkSwapchainKHR old_swapchain = vk_swapchain;

    VkSwapchainCreateInfoKHR info
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = 0,
        .imageFormat = format.format,
        .imageColorSpace = format.colorSpace,
        .imageExtent = {},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = old_swapchain,
    };

    VkSurfaceCapabilitiesKHR cap{};
    check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->get_vk_physical_device(), surface, &cap));

    if (cap.maxImageCount > 0)
        info.minImageCount = (cap.minImageCount + 2 < cap.maxImageCount) ? (cap.minImageCount + 2) : cap.maxImageCount;
    else
        info.minImageCount = cap.minImageCount + 2;

    if (cap.currentExtent.width == 0xffffffff) {

        info.imageExtent.width = size.x;
        info.imageExtent.height = size.y;

    } else {

        size.x = cap.currentExtent.width;
        size.y = cap.currentExtent.height;
        info.imageExtent.width = size.x;
        info.imageExtent.height = size.y;
    }

    info.preTransform = cap.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : cap.currentTransform;

    if (!device->vkCreateSwapchainKHR(&info,  &vk_swapchain))
        return false;

    auto backbuffer_count = 0u;
    if (!device->vkGetSwapchainImagesKHR(vk_swapchain, &backbuffer_count, nullptr))
        return false;

    VkImages images(backbuffer_count);
    if (!device->vkGetSwapchainImagesKHR(vk_swapchain, &backbuffer_count, images.data()))
        return false;

    for (auto& image : images) {

        auto backbuffer = image::make(format.format, image);
        if (!backbuffer) {

            log()->error("swapchain::create_internal backbuffer make failed");
            return false;
        }

        if (!backbuffer->create(device, size)) {

            log()->error("swapchain::create_internal backBuffer create failed");
            return false;
        }

        backbuffers.push_back(backbuffer);
    }

    if (old_swapchain)
        device->vkDestroySwapchainKHR(old_swapchain);

    return true;
}

void swapchain::destroy_internal() {

    if (!vk_swapchain)
        return;

    device->vkDestroySwapchainKHR(vk_swapchain);
    vk_swapchain = nullptr;
}

void swapchain::destroy_backbuffer_views() {

    for (auto& backBuffer : backbuffers)
        backBuffer->destroy_view();

    backbuffers.clear();
}

void swapchain::add_callback(callback* cb) { callbacks.push_back(cb); }

void swapchain::remove_callback(callback* cb) { remove(callbacks, cb); }

} // lava
