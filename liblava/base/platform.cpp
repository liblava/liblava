/**
 * @file         liblava/base/platform.cpp
 * @brief        Stage platform
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/base/instance.hpp>
#include <liblava/base/platform.hpp>

namespace lava {

//-----------------------------------------------------------------------------
device::ptr platform::create(index pd) {
    auto physical_device = &instance::get_first_physical_device();

    if (pd > 0) {
        if (pd >= instance::singleton().get_physical_devices().size()) {
            log()->error("create device - no physical device {}", pd);
            return nullptr;
        }

        physical_device = instance::singleton().get_physical_devices().at(pd).get();
    }

    if (!physical_device->swapchain_supported())
        return nullptr;

    auto param = physical_device->create_default_device_param();
    if (on_create_param)
        on_create_param(param);

    return create(param);
}

//-----------------------------------------------------------------------------
device::ptr platform::create(device::create_param::ref param) {
    auto result = std::make_shared<device>();
    if (!result->create(param))
        return nullptr;

    auto allocator = create_allocator(result.get(), param.vma_flags);
    if (!allocator)
        return nullptr;

    result->set_allocator(allocator);

    list.push_back(result);
    return result;
}

//-----------------------------------------------------------------------------
device_ptr platform::create_device(index pd) {
    auto device = create(pd);
    if (!device)
        return nullptr;

    return device.get();
}

//-----------------------------------------------------------------------------
void platform::wait_idle() {
    for (auto& device : list)
        device->wait_for_idle();
}

//-----------------------------------------------------------------------------
void platform::clear() {
    for (auto& device : list)
        device->destroy();

    list.clear();
}

} // namespace lava
