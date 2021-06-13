/**
 * @file         liblava/frame/renderer.cpp
 * @brief        Plain renderer
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <array>
#include <liblava/frame/renderer.hpp>

namespace lava {

//-----------------------------------------------------------------------------
bool renderer::create(swapchain* t) {
    for (auto& queue : t->get_device()->get_graphics_queues()) {
        if (t->surface_supported(queue.family)) {
            graphics_queue = queue;
            break;
        }
    }

    if (!graphics_queue.valid())
        return false;

    target = t;
    device = target->get_device();

    queued_frames = target->get_backbuffer_count();

    fences.resize(queued_frames);
    fences_in_use.resize(queued_frames, 0);
    image_acquired_semaphores.resize(queued_frames);
    render_complete_semaphores.resize(queued_frames);

    for (auto i = 0u; i < queued_frames; ++i) {
        {
            VkFenceCreateInfo const create_info{
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .flags = VK_FENCE_CREATE_SIGNALED_BIT,
            };

            if (!device->vkCreateFence(&create_info, &fences[i]))
                return false;
        }

        {
            VkSemaphoreCreateInfo const create_info{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            };

            if (!device->vkCreateSemaphore(&create_info, &image_acquired_semaphores[i]))
                return false;

            if (!device->vkCreateSemaphore(&create_info, &render_complete_semaphores[i]))
                return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
void renderer::destroy() {
    if (on_destroy)
        on_destroy();

    for (auto i = 0u; i < queued_frames; ++i) {
        device->vkDestroyFence(fences[i]);
        device->vkDestroySemaphore(image_acquired_semaphores[i]);
        device->vkDestroySemaphore(render_complete_semaphores[i]);
    }

    fences.clear();
    fences_in_use.clear();
    image_acquired_semaphores.clear();
    render_complete_semaphores.clear();

    queued_frames = 0;
}

//-----------------------------------------------------------------------------
optional_index renderer::begin_frame() {
    if (!active)
        return {};

    std::array<VkFence, 1> const wait_fences = { fences[current_sync] };

    for (;;) {
        auto result = device->vkWaitForFences(to_ui32(wait_fences.size()), wait_fences.data(), VK_TRUE, 100);
        if (result)
            break;

        if (result.value == VK_TIMEOUT)
            continue;

        if (result.value == VK_ERROR_OUT_OF_DATE_KHR) {
            target->request_reload();
            return {};
        }

        if (!result)
            return {};
    }

    auto current_semaphore = image_acquired_semaphores[current_sync];

    auto result = device->vkAcquireNextImageKHR(target->get(), UINT64_MAX, current_semaphore, 0, &current_frame);
    if (result.value == VK_ERROR_OUT_OF_DATE_KHR) {
        target->request_reload();
        return {};
    }

    // because frames might not come in sequential order current frame might still be locked
    if ((fences_in_use[current_frame] != 0) && (fences_in_use[current_frame] != fences[current_sync])) {
        auto result = device->vkWaitForFences(1, &fences_in_use[current_frame], VK_TRUE, UINT64_MAX);

        if (result.value == VK_ERROR_OUT_OF_DATE_KHR) {
            target->request_reload();
            return {};
        }

        if (!result)
            return {};
    }

    fences_in_use[current_frame] = fences[current_sync];

    if (!result)
        return {};

    if (!device->vkResetFences(to_ui32(wait_fences.size()), wait_fences.data()))
        return {};

    return get_frame();
}

//-----------------------------------------------------------------------------
bool renderer::end_frame(VkCommandBuffers const& cmd_buffers) {
    assert(!cmd_buffers.empty());

    std::array<VkSemaphore, 1> const wait_semaphores = { image_acquired_semaphores[current_sync] };
    std::array<VkSemaphore, 1> const sync_present_semaphores = { render_complete_semaphores[current_sync] };

    VkPipelineStageFlags const wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo const submit_info{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = to_ui32(wait_semaphores.size()),
        .pWaitSemaphores = wait_semaphores.data(),
        .pWaitDstStageMask = &wait_stage,
        .commandBufferCount = to_ui32(cmd_buffers.size()),
        .pCommandBuffers = cmd_buffers.data(),
        .signalSemaphoreCount = to_ui32(sync_present_semaphores.size()),
        .pSignalSemaphores = sync_present_semaphores.data(),
    };

    std::array<VkSubmitInfo, 1> const submit_infos = { submit_info };
    VkFence current_fence = fences[current_sync];

    if (!device->vkQueueSubmit(graphics_queue.vk_queue, to_ui32(submit_infos.size()), submit_infos.data(), current_fence))
        return false;

    std::array<VkSwapchainKHR, 1> const swapchains = { target->get() };
    std::array<ui32, 1> const indices = { current_frame };

    VkPresentInfoKHR const present_info{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = to_ui32(sync_present_semaphores.size()),
        .pWaitSemaphores = sync_present_semaphores.data(),
        .swapchainCount = to_ui32(swapchains.size()),
        .pSwapchains = swapchains.data(),
        .pImageIndices = indices.data(),
    };

    auto result = device->vkQueuePresentKHR(graphics_queue.vk_queue, &present_info);
    if (result.value == VK_ERROR_OUT_OF_DATE_KHR) {
        target->request_reload();
        return true;
    }

    if (!result)
        return false;

    current_sync = (current_sync + 1) % queued_frames;

    return true;
}

} // namespace lava
