// file      : liblava/block/descriptor.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device.hpp>

namespace lava {

    struct descriptor : id_obj {
        struct binding : id_obj {
            using ptr = std::shared_ptr<binding>;
            using list = std::vector<ptr>;

            explicit binding();

            VkDescriptorSetLayoutBinding const& get() const {
                return vk_binding;
            }

            void set(index index) {
                vk_binding.binding = index;
            }

            void set_type(VkDescriptorType descriptor_type) {
                vk_binding.descriptorType = descriptor_type;
            }

            void set_count(ui32 descriptor_count) {
                vk_binding.descriptorCount = descriptor_count;
            }

            void set_stage_flags(VkShaderStageFlags stage_flags) {
                vk_binding.stageFlags = stage_flags;
            }

            void set_samplers(VkSampler const* immutable_samplers) {
                vk_binding.pImmutableSamplers = immutable_samplers;
            }

        private:
            VkDescriptorSetLayoutBinding vk_binding;
        };

        struct pool : id_obj {
            using ptr = std::shared_ptr<pool>;
            using list = std::vector<ptr>;

            bool create(device_ptr device, VkDescriptorPoolSizesRef sizes, ui32 max = 1,
                        VkDescriptorPoolCreateFlags flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
            void destroy();

            VkDescriptorPool get() const {
                return vk_pool;
            }
            device_ptr get_device() {
                return device;
            }

            VkDescriptorPoolSizes const& get_sizes() const {
                return sizes;
            }

            ui32 get_max() const {
                return max;
            }

        private:
            device_ptr device = nullptr;

            VkDescriptorPool vk_pool = VK_NULL_HANDLE;

            VkDescriptorPoolSizes sizes;
            ui32 max = 0;
        };

        using ptr = std::shared_ptr<descriptor>;
        using list = std::vector<ptr>;

        void add_binding(index binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags);
        void clear_bindings() {
            bindings.clear();
        }

        void add(binding::ptr const& binding) {
            bindings.push_back(binding);
        }

        bool create(device_ptr device);
        void destroy();

        ui32 get_binding_count() const {
            return to_ui32(bindings.size());
        }
        binding::list const& get_bindings() {
            return bindings;
        }

        VkDescriptorSetLayout get() const {
            return layout;
        }
        device_ptr get_device() {
            return device;
        }

        VkDescriptorSet allocate_set(VkDescriptorPool pool);
        VkDescriptorSet allocate(VkDescriptorPool pool) {
            return allocate_set(pool);
        }

        bool free_set(VkDescriptorSet& descriptor_set, VkDescriptorPool pool);
        bool free(VkDescriptorSet& descriptor_set, VkDescriptorPool pool) {
            return free_set(descriptor_set, pool);
        }

        VkDescriptorSets allocate_sets(ui32 size, VkDescriptorPool pool);
        VkDescriptorSets allocate(ui32 size, VkDescriptorPool pool) {
            return allocate_sets(size, pool);
        }

        bool free_sets(VkDescriptorSets& descriptor_sets, VkDescriptorPool pool);
        bool free(VkDescriptorSets& descriptor_sets, VkDescriptorPool pool) {
            return free_sets(descriptor_sets, pool);
        }

    private:
        device_ptr device = nullptr;

        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        binding::list bindings;
    };

    inline descriptor::ptr make_descriptor() {
        return std::make_shared<descriptor>();
    }

    descriptor::binding::ptr make_descriptor_binding(index index);

    inline descriptor::pool::ptr make_descriptor_pool() {
        return std::make_shared<descriptor::pool>();
    }

} // namespace lava
