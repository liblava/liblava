/**
 * @file         liblava/frame/swapchain.cpp
 * @brief        Swapchain
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/frame/swapchain.hpp"
#include "liblava/base/instance.hpp"
#include "liblava/core/misc.hpp"
#include "liblava/util/log.hpp"

namespace lava {

//-----------------------------------------------------------------------------
bool swapchain::create(device::ptr dev,
                       VkSurfaceKHR s,
                       VkSurfaceFormatKHR f,
                       uv2 sz,
                       bool v,
                       bool t) {
    m_device = dev;
    m_surface = s;
    m_format = f;
    m_size = sz;
    m_v_sync_active = v;
    m_triple_buffer_active = t;

    return setup();
}

//-----------------------------------------------------------------------------
void swapchain::destroy() {
    m_device->wait_for_idle();

    destroy_backbuffer_views();
    teardown();

    vkDestroySurfaceKHR(instance::singleton().get(),
                        m_surface,
                        memory::instance().alloc());
    m_surface = VK_NULL_HANDLE;
}

//-----------------------------------------------------------------------------
bool swapchain::resize(uv2 new_size) {
    m_device->wait_for_idle();

    if (!m_backbuffers.empty()) {
        for (auto& callback : m_callbacks)
            callback->on_destroyed();

        destroy_backbuffer_views();
    }

    m_size = new_size;
    if (m_size.x == 0 || m_size.y == 0)
        return true;

    auto result = setup();
    LAVA_ASSERT(result);
    if (!result)
        return false;

    for (auto& callback : reverse(m_callbacks)) {
        result = callback->on_created();
        LAVA_ASSERT(result);
        if (!result)
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
VkPresentModeKHR swapchain::choose_present_mode(
    VkPresentModeKHRs const& present_modes) const {
    if (v_sync())
        return VK_PRESENT_MODE_FIFO_KHR;

    if (triple_buffer()) {
        if (std::find(present_modes.begin(), present_modes.end(),
                      VK_PRESENT_MODE_MAILBOX_KHR)
            != present_modes.end())
            return VK_PRESENT_MODE_MAILBOX_KHR;

        if (std::find(present_modes.begin(), present_modes.end(),
                      VK_PRESENT_MODE_IMMEDIATE_KHR)
            != present_modes.end())
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
    } else {
        if (std::find(present_modes.begin(), present_modes.end(),
                      VK_PRESENT_MODE_IMMEDIATE_KHR)
            != present_modes.end())
            return VK_PRESENT_MODE_IMMEDIATE_KHR;

        if (std::find(present_modes.begin(), present_modes.end(),
                      VK_PRESENT_MODE_MAILBOX_KHR)
            != present_modes.end())
            return VK_PRESENT_MODE_MAILBOX_KHR;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

//-----------------------------------------------------------------------------
VkSwapchainCreateInfoKHR swapchain::create_info(VkPresentModeKHRs present_modes) {
    VkPresentModeKHR present_mode = choose_present_mode(present_modes);

    VkSwapchainCreateInfoKHR info{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = m_surface,
        .minImageCount = 0,
        .imageFormat = m_format.format,
        .imageColorSpace = m_format.colorSpace,
        .imageExtent = {},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = m_vk_swapchain,
    };

    VkSurfaceCapabilitiesKHR cap{};
    check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        m_device->get_vk_physical_device(), m_surface, &cap));

    info.minImageCount = cap.minImageCount + 1;
    if ((cap.maxImageCount > 0) && (info.minImageCount > cap.maxImageCount))
        info.minImageCount = cap.maxImageCount;

    if (cap.currentExtent.width == 0xffffffff) {
        info.imageExtent.width = m_size.x;
        info.imageExtent.height = m_size.y;
    } else {
        m_size.x = cap.currentExtent.width;
        m_size.y = cap.currentExtent.height;
        info.imageExtent.width = m_size.x;
        info.imageExtent.height = m_size.y;
    }

    info.preTransform = cap.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
                            ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
                            : cap.currentTransform;

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
bool swapchain::setup() {
    auto present_mode_count = 0u;
    if ((vkGetPhysicalDeviceSurfacePresentModesKHR(
             m_device->get_vk_physical_device(),
             m_surface, &present_mode_count, nullptr)
         != VK_SUCCESS)
        || (present_mode_count == 0)) {
        logger()->error("create swapchain present mode count");
        return false;
    }

    VkPresentModeKHRs present_modes(present_mode_count);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_device->get_vk_physical_device(),
            m_surface, &present_mode_count, present_modes.data())
        != VK_SUCCESS) {
        logger()->error("create swapchain present mode");
        return false;
    }

    VkSwapchainKHR old_swapchain = m_vk_swapchain;

    auto info = create_info(present_modes);
    if (!m_device->vkCreateSwapchainKHR(&info,
                                        &m_vk_swapchain))
        return false;

    auto backbuffer_count = 0u;
    if (!m_device->vkGetSwapchainImagesKHR(m_vk_swapchain,
                                           &backbuffer_count,
                                           nullptr))
        return false;

    VkImages images(backbuffer_count);
    if (!m_device->vkGetSwapchainImagesKHR(m_vk_swapchain,
                                           &backbuffer_count,
                                           images.data()))
        return false;

    for (auto& image : images) {
        auto backbuffer = image::make(m_format.format, image);
        if (!backbuffer) {
            logger()->error("setup swapchain backbuffer image");
            return false;
        }

        if (!backbuffer->create(m_device, m_size)) {
            logger()->error("setup swapchain backBuffer");
            return false;
        }

        m_backbuffers.push_back(backbuffer);
    }

    if (old_swapchain)
        m_device->vkDestroySwapchainKHR(old_swapchain);

    m_reload_request_active = false;

    return true;
}

//-----------------------------------------------------------------------------
void swapchain::teardown() {
    if (!m_vk_swapchain)
        return;

    m_device->vkDestroySwapchainKHR(m_vk_swapchain);
    m_vk_swapchain = VK_NULL_HANDLE;
}

//-----------------------------------------------------------------------------
void swapchain::destroy_backbuffer_views() {
    for (auto& backBuffer : m_backbuffers)
        backBuffer->destroy_view();

    m_backbuffers.clear();
}

//-----------------------------------------------------------------------------
void swapchain::add_callback(callback* cb) {
    m_callbacks.push_back(cb);
}

//-----------------------------------------------------------------------------
void swapchain::remove_callback(callback* cb) {
    remove(m_callbacks, cb);
}

//-----------------------------------------------------------------------------
bool swapchain::surface_supported(index queue_family) const {
    return m_device->get_physical_device()->surface_supported(queue_family,
                                                              m_surface);
}

} // namespace lava
