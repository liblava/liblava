/**
 * @file         liblava/frame/swapchain.cpp
 * @brief        Swapchain
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/base/instance.hpp>
#include <liblava/frame/swapchain.hpp>

namespace lava {

//-----------------------------------------------------------------------------
bool swapchain::create(device_ptr d, VkSurfaceKHR s, VkSurfaceFormatKHR f, uv2 sz, bool v) {
    device = d;
    surface = s;
    format = f;
    size = sz;
    v_sync_active = v;

    return create_internal();
}

//-----------------------------------------------------------------------------
void swapchain::destroy() {
    device->wait_for_idle();

    destroy_backbuffer_views();
    destroy_internal();

    vkDestroySurfaceKHR(instance::get(), surface, memory::alloc());
    surface = VK_NULL_HANDLE;
}

//-----------------------------------------------------------------------------
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
#if LIBLAVA_DEBUG_ASSERT
    assert(result);
#endif
    if (!result)
        return false;

    for (auto& callback : reverse(callbacks)) {
        result = callback->on_created();
#if LIBLAVA_DEBUG_ASSERT
        assert(result);
#endif
        if (!result)
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
VkPresentModeKHR swapchain::choose_present_mode(VkPresentModeKHRs const& present_modes) const {
    if (v_sync())
        return VK_PRESENT_MODE_FIFO_KHR;

    if (std::find(present_modes.begin(), present_modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != present_modes.end())
        return VK_PRESENT_MODE_MAILBOX_KHR;

    if (std::find(present_modes.begin(), present_modes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != present_modes.end())
        return VK_PRESENT_MODE_IMMEDIATE_KHR;

    return VK_PRESENT_MODE_FIFO_KHR;
}

//-----------------------------------------------------------------------------
VkSwapchainCreateInfoKHR swapchain::create_info(VkPresentModeKHRs present_modes) {
    VkPresentModeKHR present_mode = choose_present_mode(present_modes);

    VkSwapchainCreateInfoKHR info{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = 0,
        .imageFormat = format.format,
        .imageColorSpace = format.colorSpace,
        .imageExtent = {},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = vk_swapchain,
    };

    VkSurfaceCapabilitiesKHR cap{};
    check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->get_vk_physical_device(), surface, &cap));

    info.minImageCount = cap.minImageCount + 1;
    if ((cap.maxImageCount > 0) && (info.minImageCount > cap.maxImageCount))
        info.minImageCount = cap.maxImageCount;

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

    if (cap.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    if (cap.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    std::vector<VkCompositeAlphaFlagBitsKHR> composite_alpha_flags = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (auto& composite_alpha_flag : composite_alpha_flags) {
        if (cap.supportedCompositeAlpha & composite_alpha_flag) {
            info.compositeAlpha = composite_alpha_flag;
            break;
        };
    }

    return info;
}

//-----------------------------------------------------------------------------
bool swapchain::create_internal() {
    auto present_mode_count = 0u;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(device->get_vk_physical_device(), surface, &present_mode_count, nullptr) != VK_SUCCESS || present_mode_count == 0) {
        log()->error("create swapchain present mode count");
        return false;
    }

    VkPresentModeKHRs present_modes(present_mode_count);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(device->get_vk_physical_device(), surface, &present_mode_count, present_modes.data()) != VK_SUCCESS) {
        log()->error("create swapchain present mode");
        return false;
    }

    VkSwapchainKHR old_swapchain = vk_swapchain;

    auto info = create_info(present_modes);
    if (!device->vkCreateSwapchainKHR(&info, &vk_swapchain))
        return false;

    auto backbuffer_count = 0u;
    if (!device->vkGetSwapchainImagesKHR(vk_swapchain, &backbuffer_count, nullptr))
        return false;

    VkImages images(backbuffer_count);
    if (!device->vkGetSwapchainImagesKHR(vk_swapchain, &backbuffer_count, images.data()))
        return false;

    for (auto& image : images) {
        auto backbuffer = make_image(format.format, image);
        if (!backbuffer) {
            log()->error("create swapchain backbuffer image");
            return false;
        }

        if (!backbuffer->create(device, size)) {
            log()->error("create swapchain backBuffer");
            return false;
        }

        backbuffers.push_back(backbuffer);
    }

    if (old_swapchain)
        device->vkDestroySwapchainKHR(old_swapchain);

    reload_request_active = false;

    return true;
}

//-----------------------------------------------------------------------------
void swapchain::destroy_internal() {
    if (!vk_swapchain)
        return;

    device->vkDestroySwapchainKHR(vk_swapchain);
    vk_swapchain = VK_NULL_HANDLE;
}

//-----------------------------------------------------------------------------
void swapchain::destroy_backbuffer_views() {
    for (auto& backBuffer : backbuffers)
        backBuffer->destroy_view();

    backbuffers.clear();
}

//-----------------------------------------------------------------------------
void swapchain::add_callback(callback* cb) {
    callbacks.push_back(cb);
}

//-----------------------------------------------------------------------------
void swapchain::remove_callback(callback* cb) {
    remove(callbacks, cb);
}

//-----------------------------------------------------------------------------
bool swapchain::surface_supported(index queue_family) const {
    return device->get_physical_device()->surface_supported(queue_family, surface);
}

} // namespace lava
