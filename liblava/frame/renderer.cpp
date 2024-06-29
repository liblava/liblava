/**
 * @file         liblava/frame/renderer.cpp
 * @brief        Plain renderer
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/frame/renderer.hpp"
#include "liblava/core/misc.hpp"
#include <array>

namespace lava {

//-----------------------------------------------------------------------------
bool renderer::create(swapchain* t) {
    for (auto& queue : t->get_device()->get_graphics_queues()) {
        if (t->surface_supported(queue.family)) {
            m_graphics_queue = queue;
            break;
        }
    }

    if (!m_graphics_queue.valid())
        return false;

    m_target = t;
    m_device = m_target->get_device();

    m_queued_frames = m_target->get_backbuffer_count();

    m_fences.resize(m_queued_frames);
    m_fences_in_use.resize(m_queued_frames, 0);
    m_image_acquired_semaphores.resize(m_queued_frames);
    m_render_complete_semaphores.resize(m_queued_frames);

    for (auto i = 0u; i < m_queued_frames; ++i) {
        {
            VkFenceCreateInfo const create_info{
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .flags = VK_FENCE_CREATE_SIGNALED_BIT,
            };

            if (!m_device->vkCreateFence(&create_info, &m_fences[i]))
                return false;
        }

        {
            VkSemaphoreCreateInfo const create_info{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            };

            if (!m_device->vkCreateSemaphore(&create_info,
                                             &m_image_acquired_semaphores[i]))
                return false;

            if (!m_device->vkCreateSemaphore(&create_info,
                                             &m_render_complete_semaphores[i]))
                return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
void renderer::destroy() {
    if (on_destroy)
        on_destroy();

    for (auto i = 0u; i < m_queued_frames; ++i) {
        m_device->vkDestroyFence(m_fences[i]);
        m_device->vkDestroySemaphore(m_image_acquired_semaphores[i]);
        m_device->vkDestroySemaphore(m_render_complete_semaphores[i]);
    }

    m_fences.clear();
    m_fences_in_use.clear();
    m_image_acquired_semaphores.clear();
    m_render_complete_semaphores.clear();

    m_queued_frames = 0;
}

//-----------------------------------------------------------------------------
optional_index renderer::begin_frame() {
    if (!active)
        return std::nullopt;

    std::array<VkFence, 1> const wait_fences = {m_fences[m_current_sync]};

    for (;;) {
        auto result = m_device->vkWaitForFences(to_ui32(wait_fences.size()),
                                                wait_fences.data(),
                                                VK_TRUE,
                                                100);
        if (result)
            break;

        if (result.value == VK_TIMEOUT)
            continue;

        if (result.value == VK_ERROR_OUT_OF_DATE_KHR) {
            m_target->request_reload();
            return std::nullopt;
        }

        if (!result)
            return std::nullopt;
    }

    auto current_semaphore = m_image_acquired_semaphores[m_current_sync];

    auto result = m_device->vkAcquireNextImageKHR(m_target->get(),
                                                  UINT64_MAX,
                                                  current_semaphore,
                                                  0,
                                                  &m_current_frame);
    if (result.value == VK_ERROR_OUT_OF_DATE_KHR || result.value == VK_SUBOPTIMAL_KHR) {
        m_target->request_reload();
        return std::nullopt;
    }

    // because frames might not come in sequential order
    // current frame might still be locked
    if ((m_fences_in_use[m_current_frame] != 0)
        && (m_fences_in_use[m_current_frame] != m_fences[m_current_sync])) {
        auto result = m_device->vkWaitForFences(1,
                                                &m_fences_in_use[m_current_frame],
                                                VK_TRUE,
                                                UINT64_MAX);

        if (result.value == VK_ERROR_OUT_OF_DATE_KHR) {
            m_target->request_reload();
            return std::nullopt;
        }

        if (!result)
            return std::nullopt;
    }

    m_fences_in_use[m_current_frame] = m_fences[m_current_sync];

    if (!result)
        return std::nullopt;

    if (!m_device->vkResetFences(to_ui32(wait_fences.size()),
                                 wait_fences.data()))
        return std::nullopt;

    return get_frame();
}

//-----------------------------------------------------------------------------
bool renderer::end_frame(VkCommandBuffers const& cmd_buffers) {
    LAVA_ASSERT(!cmd_buffers.empty());

    VkSemaphores wait_semaphores = {
        m_image_acquired_semaphores[m_current_sync]};
    if (!user_frame_wait_semaphores.empty())
        append(wait_semaphores, user_frame_wait_semaphores);

    VkPipelineStageFlagsList wait_stages = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    if (!user_frame_wait_stages.empty())
        append(wait_stages, user_frame_wait_stages);

    std::array<VkSemaphore, 1> const sync_present_semaphores = {
        m_render_complete_semaphores[m_current_sync]};

    VkSemaphores signal_semaphores = {
        m_render_complete_semaphores[m_current_sync]};
    if (!user_frame_signal_semaphores.empty())
        append(signal_semaphores, user_frame_signal_semaphores);

    LAVA_ASSERT(user_frame_wait_semaphores.size() == user_frame_wait_stages.size());

    VkSubmitInfo const submit_info{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = to_ui32(wait_semaphores.size()),
        .pWaitSemaphores = wait_semaphores.data(),
        .pWaitDstStageMask = wait_stages.data(),
        .commandBufferCount = to_ui32(cmd_buffers.size()),
        .pCommandBuffers = cmd_buffers.data(),
        .signalSemaphoreCount = to_ui32(signal_semaphores.size()),
        .pSignalSemaphores = signal_semaphores.data(),
    };

    std::array<VkSubmitInfo, 1> const submit_infos = {submit_info};
    VkFence current_fence = m_fences[m_current_sync];
    if (!m_device->vkQueueSubmit(m_graphics_queue.vk_queue,
                                 to_ui32(submit_infos.size()),
                                 submit_infos.data(),
                                 current_fence))
        return false;

    std::array<VkSwapchainKHR, 1> const swapchains = {m_target->get()};
    std::array<ui32, 1> const indices = {m_current_frame};

    VkPresentInfoKHR const present_info{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = to_ui32(sync_present_semaphores.size()),
        .pWaitSemaphores = sync_present_semaphores.data(),
        .swapchainCount = to_ui32(swapchains.size()),
        .pSwapchains = swapchains.data(),
        .pImageIndices = indices.data(),
    };

    auto result = m_device->vkQueuePresentKHR(m_graphics_queue.vk_queue, &present_info);
    if (result.value == VK_ERROR_OUT_OF_DATE_KHR || result.value == VK_SUBOPTIMAL_KHR) {
        m_target->request_reload();
        return true;
    }

    if (!result)
        return false;

    m_current_sync = (m_current_sync + 1) % m_queued_frames;

    return true;
}

} // namespace lava
