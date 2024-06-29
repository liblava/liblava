/**
 * @file         liblava/block/descriptor.hpp
 * @brief        Descriptor definition
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/base/device.hpp"

namespace lava {

/**
 * @brief Descriptor
 */
struct descriptor : entity {
    /**
     * @brief Descriptor binding
     */
    struct binding {
        /// Shared pointer to binding
        using s_ptr = std::shared_ptr<binding>;

        /// List of bindings
        using s_list = std::vector<s_ptr>;

        /**
         * @brief Make a new descriptor binding
         * @param index    Binding index
         * @return ptr     Shared pointer to descriptor binding
         */
        static s_ptr make(index index) {
            auto result = std::make_shared<binding>();
            result->set(index);
            result->set_count(1);
            return result;
        }

        /**
         * @brief Construct a new binding
         */
        explicit binding();

        /**
         * @brief Get the Vulkan descriptor set layout binding
         * @return VkDescriptorSetLayoutBinding const&    Vulkan binding
         */
        VkDescriptorSetLayoutBinding const& get() const {
            return m_vk_binding;
        }

        /**
         * @brief Det the binding index
         * @param index    Binding index
         */
        void set(index index) {
            m_vk_binding.binding = index;
        }

        /**
         * @brief Set the type
         * @param descriptor_type    Descriptor type
         */
        void set_type(VkDescriptorType descriptor_type) {
            m_vk_binding.descriptorType = descriptor_type;
        }

        /**
         * @brief Set the count
         * @param descriptor_count    Descriptor count
         */
        void set_count(ui32 descriptor_count) {
            m_vk_binding.descriptorCount = descriptor_count;
        }

        /**
         * @brief Set the stage flags
         * @param stage_flags    Shader stage flags
         */
        void set_stage_flags(VkShaderStageFlags stage_flags) {
            m_vk_binding.stageFlags = stage_flags;
        }

        /**
         * @brief Set the samplers
         * @param immutable_samplers    Pointer to immutable samplers
         */
        void set_samplers(VkSampler const* immutable_samplers) {
            m_vk_binding.pImmutableSamplers = immutable_samplers;
        }

    private:
        /// Vulkan binding
        VkDescriptorSetLayoutBinding m_vk_binding;
    };

    /**
     * @brief Descriptor pool
     */
    struct pool : entity {
        /// Shared pointer to pool
        using s_ptr = std::shared_ptr<pool>;

        /// List of pools
        using s_list = std::vector<s_ptr>;

        /**
         * @brief Make a new descriptor pool
         * @return s_ptr    Shared pointer to descriptor pool
         */
        static s_ptr make() {
            return std::make_shared<descriptor::pool>();
        }

        /**
         * @brief Create a new pool
         * @param device    Vulkan device
         * @param sizes     Descriptor pool sizes
         * @param max       Number of pools
         * @param flags     Create flags
         * @return Create was successful or failed
         */
        bool create(device::ptr device,
                    VkDescriptorPoolSizesRef sizes,
                    ui32 max = 1,
                    VkDescriptorPoolCreateFlags flags =
                        VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

        /**
         * @brief Destroy the pool
         */
        void destroy();

        /**
         * @brief Get the descriptor pool
         * @return VkDescriptorPool    Vulkan descriptor pool
         */
        VkDescriptorPool get() const {
            return m_vk_pool;
        }

        /**
         * @brief Get the device
         * @return device::ptr    Vulkan device
         */
        device::ptr get_device() {
            return m_device;
        }

        /**
         * @brief Get the sizes
         * @return VkDescriptorPoolSizes const&    Descriptor pool sizes
         */
        VkDescriptorPoolSizes const& get_sizes() const {
            return m_sizes;
        }

        /**
         * @brief Get the max
         * @return ui32    Number of pools
         */
        ui32 get_max() const {
            return m_max;
        }

    private:
        /// Vulkan device
        device::ptr m_device = nullptr;

        /// Vulkan descriptor pool
        VkDescriptorPool m_vk_pool = VK_NULL_HANDLE;

        /// Descritpr pool sizes
        VkDescriptorPoolSizes m_sizes;

        /// Number of pools
        ui32 m_max = 0;
    };

    /// Shared pointer to descriptor
    using s_ptr = std::shared_ptr<descriptor>;

    /// List of descriptors
    using s_list = std::vector<s_ptr>;

    /**
     * @brief Make a new descriptor
     * @return s_ptr    Shared pointer to descriptor
     */
    static s_ptr make() {
        return std::make_shared<descriptor>();
    }

    /**
     * @brief Add binding
     * @param binding            Index of binding
     * @param descriptor_type    Descriptor type
     * @param stage_flags        Shader stage flags
     */
    void add_binding(index binding,
                     VkDescriptorType descriptor_type,
                     VkShaderStageFlags stage_flags);

    /**
     * @brief Clear bindings
     */
    void clear_bindings() {
        m_bindings.clear();
    }

    /**
     * @brief Add binding
     * @param binding    Descriptor binding
     */
    void add(binding::s_ptr const& binding) {
        m_bindings.push_back(binding);
    }

    /**
     * @brief Create a new descriptor
     * @param device    Vulkan device
     * @return Create was successful or failed
     */
    bool create(device::ptr device);

    /**
     * @brief Destroy the descriptor
     */
    void destroy();

    /**
     * @brief Get the binding count
     * @return ui32    Number of bindings
     */
    ui32 get_binding_count() const {
        return to_ui32(m_bindings.size());
    }

    /**
     * @brief Get the bindings
     * @return binding::s_list const&    List of bindings
     */
    binding::s_list const& get_bindings() {
        return m_bindings;
    }

    /**
     * @brief Get descriptor set layout
     * @return VkDescriptorSetLayout    Vulkan descriptor set layout
     */
    VkDescriptorSetLayout get() const {
        return m_layout;
    }

    /**
     * @brief Get the device
     * @return device::ptr    Vulkan device
     */
    device::ptr get_device() {
        return m_device;
    }

    /**
     * @brief Allocate descriptor set
     * @param pool                Descriptor pool
     * @return VkDescriptorSet    Descriptor set
     */
    VkDescriptorSet allocate_set(VkDescriptorPool pool);

    /**
     * @see allocate_set
     */
    VkDescriptorSet allocate(VkDescriptorPool pool) {
        return allocate_set(pool);
    }

    /**
     * @brief Deallocate descriptor set
     * @param descriptor_set    Descriptor set
     * @param pool              Descriptor pool
     * @return Deallocate was successful or failed
     */
    bool deallocate_set(VkDescriptorSet& descriptor_set,
                        VkDescriptorPool pool);

    /**
     * @see deallocate_set
     */
    bool deallocate(VkDescriptorSet& descriptor_set,
                    VkDescriptorPool pool) {
        return deallocate_set(descriptor_set, pool);
    }

    /**
     * @brief Allocate descriptor sets
     * @param size                 Number of sets
     * @param pool                 Descriptor pool
     * @return VkDescriptorSets    List of descriptor sets
     */
    VkDescriptorSets allocate_sets(ui32 size,
                                   VkDescriptorPool pool);

    /**
     * @see allocate_sets
     */
    VkDescriptorSets allocate(ui32 size,
                              VkDescriptorPool pool) {
        return allocate_sets(size, pool);
    }

    /**
     * @brief Deallocate descriptor sets
     * @param descriptor_sets    List of descriptor sets
     * @param pool               Descriptor pool
     * @return Deallocate was successful or failed
     */
    bool deallocate_sets(VkDescriptorSets& descriptor_sets,
                         VkDescriptorPool pool);

    /**
     * @see deallocate_sets
     */
    bool deallocate(VkDescriptorSets& descriptor_sets,
                    VkDescriptorPool pool) {
        return deallocate_sets(descriptor_sets, pool);
    }

private:
    /// Vulkan device
    device::ptr m_device = nullptr;

    /// Vulkan descriptor set layout
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;

    /// List of descriptor bindings
    binding::s_list m_bindings;
};

} // namespace lava
