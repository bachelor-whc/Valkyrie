#ifndef _VULKAN_DESCRIPTOR_SET_LAYOUT_H
#define _VULKAN_DESCRIPTOR_SET_LAYOUT_H
#include "common.h"

namespace Vulkan {
	struct Device;

	struct DescriptorSetLayout {
		void setBinding(const uint32_t shader_binding, const VkDescriptorType type, const VkShaderStageFlags flag, const uint32_t count);
		VkResult initialize();

		VkDescriptorSetLayout handle = VK_NULL_HANDLE;
	private:
		std::vector<VkDescriptorSetLayoutBinding> m_bindings;
	};

	struct DescriptorPool {
		DescriptorPool() = delete;
		DescriptorPool(uint32_t max_sets);
		~DescriptorPool();

		VkResult initializePool();
		VkResult initializeSets();
		VkResult initializeSetLayouts();
		VkDescriptorSet& registerSet(const std::string& name, const int index);
		VkDescriptorSet& getSet(const std::string& name);
		inline VkDescriptorSet* getSets() { return m_sets.data(); }
		inline int32_t getSetsSize() { return m_sets.size(); }
		DescriptorSetLayout& registerSetLayout(const std::string& name, const int index);
		DescriptorSetLayout& getSetLayout(const std::string& name);
		void addPoolSize(VkDescriptorType type, uint32_t count);
		std::vector<VkDescriptorSetLayout> getSetLayoutHandles();

		VkDescriptorPool handle = VK_NULL_HANDLE;

	private:
		uint32_t m_max_sets;
		uint32_t m_sets_size;
		std::vector<VkDescriptorPoolSize> m_unit_sizes;
		std::vector<VkDescriptorSet> m_sets;
		std::map<std::string, int> m_sets_map;
		std::vector<DescriptorSetLayout*> m_set_layouts;
		std::map<std::string, int> m_set_layouts_map;
	};
}

#endif