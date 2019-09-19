// file      : liblava/frame/renderer.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/frame/renderer.hpp>

#include <array>

namespace lava {

bool renderer::create(swapchain* target_) {

    target = target_;
    dev = target->get_device();

    queue = dev->get_graphics_queue();
    queued_frames = target->get_backbuffer_count();

    fences.resize(queued_frames);
    image_acquired_semaphores.resize(queued_frames);
    render_complete_semaphores.resize(queued_frames);

    for (auto i = 0u; i < queued_frames; ++i) {

        {
            VkFenceCreateInfo create_info
            {
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .flags = VK_FENCE_CREATE_SIGNALED_BIT,
            };

            if (!dev->vkCreateFence(&create_info, memory::alloc(), &fences[i]))
                return false;
        }

        {
            VkSemaphoreCreateInfo create_info
            {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            };

            if (!dev->vkCreateSemaphore(&create_info, memory::alloc(), &image_acquired_semaphores[i]))
                return false;

            if (!dev->vkCreateSemaphore(&create_info, memory::alloc(), &render_complete_semaphores[i]))
                return false;
        }
    }

    return true;
}

void renderer::destroy() {

    if (on_destroy)
        on_destroy();

    for (auto i = 0u; i < queued_frames; ++i) {

        dev->vkDestroyFence(fences[i], memory::alloc());
        dev->vkDestroySemaphore(image_acquired_semaphores[i], memory::alloc());
        dev->vkDestroySemaphore(render_complete_semaphores[i], memory::alloc());
    }

    fences.clear();
    image_acquired_semaphores.clear();
    render_complete_semaphores.clear();

    queued_frames = 0;
}

std::optional<index> renderer::begin_frame() {

    if (!active)
        return {};

    std::array<VkFence, 1> const wait_fences = { fences[current_sync] };

    for (;;) {

        auto result = dev->vkWaitForFences(to_ui32(wait_fences.size()), wait_fences.data(), VK_TRUE, 100);
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

    auto result = dev->vkAcquireNextImageKHR(target->get(), UINT64_MAX, current_semaphore, nullptr, &frame_index);
    if (result.value == VK_ERROR_OUT_OF_DATE_KHR) {

        target->request_reload();
        return {};
    }

    if (!result)
        return {};

    if (!dev->vkResetFences(to_ui32(wait_fences.size()), wait_fences.data()))
        return {};

    return get_current_frame();
}

bool renderer::end_frame(VkCommandBuffers const& cmd_buffers) {

    assert(!cmd_buffers.empty());

    std::array<VkSemaphore, 1> const wait_semaphores = { image_acquired_semaphores[current_sync] };
    std::array<VkSemaphore, 1> const sync_present_semaphores = { render_complete_semaphores[current_sync] };

    VkPipelineStageFlags const wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit_info
    {
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

    if (!dev->vkQueueSubmit(queue.vk_queue, to_ui32(submit_infos.size()), submit_infos.data(), current_fence))
        return false;

    std::array<VkSwapchainKHR, 1> const swapchains = { target->get() };
    std::array<ui32, 1> const indices = { frame_index };

    VkPresentInfoKHR present_info
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = to_ui32(sync_present_semaphores.size()),
        .pWaitSemaphores = sync_present_semaphores.data(),
        .swapchainCount = to_ui32(swapchains.size()),
        .pSwapchains = swapchains.data(),
        .pImageIndices = indices.data(),
    };

    if (!dev->vkQueuePresentKHR(queue.vk_queue, &present_info))
        return false;

    current_sync = (current_sync + 1) % queued_frames;

    return true;
}

} // lava
