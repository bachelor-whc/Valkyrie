#ifndef _VULKAN_DESCRIPTOR_SET_LAYOUT_H
#define _VULKAN_DESCRIPTOR_SET_LAYOUT_H
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace Vulkan {
	struct Device;
	struct SetUpdable;

	struct DescriptorSetLayout {
		void setBinding(const uint32_t shader_binding, const VkDescriptorType type, const VkShaderStageFlags flag, const uint32_t count);
		VkResult initialize();

		VkDescriptorSetLayout handle = VK_NULL_HANDLE;
	private:
		std::vector<VkDescriptorSetLayoutBinding> m_bindings;
	};

	struct DescriptorPool {
		DescriptorPool();
		~DescriptorPool();

		void addPoolSize(VkDescriptorType type, uint32_t count);
		VkResult initializePool(uint32_t max_set);
		VkResult initializeSets();
		std::vector<VkDescriptorSetLayout> getSetLayoutHandles();
		void updateDescriptorSet(SetUpdable& write, const uint32_t set_index, const uint32_t binding);

		VkDescriptorPool handle = VK_NULL_HANDLE;
		std::vector<DescriptorSetLayout> setLayouts;
		std::vector<VkDescriptorSet> sets;
	private:
		VkResult initializeSetLayouts();
		uint32_t m_max_sets = 0;
		std::vector<VkDescriptorPoolSize> m_pool_sizes;
	};

	using DescriptorPoolPtr = std::shared_ptr<DescriptorPool>;
}

#endif