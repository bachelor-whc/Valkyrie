#include "valkyrie/vulkan/descriptor.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/utility/vulkan_manager.h"
using namespace Vulkan;

DescriptorPool::DescriptorPool() : 
	setLayouts(),
	sets(),
	m_pool_sizes() {
	
}

DescriptorPool::~DescriptorPool() {
	
}


VkResult DescriptorPool::initializePool(uint32_t max_set) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	m_max_sets = max_set;
	setLayouts.resize(m_max_sets);
	sets.resize(m_max_sets);
	VkDescriptorPoolCreateInfo descriptor_pool_create = {};
	descriptor_pool_create.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_create.maxSets = m_max_sets;
	descriptor_pool_create.poolSizeCount = (uint32_t)m_pool_sizes.size();
	descriptor_pool_create.pPoolSizes = m_pool_sizes.data();
	return vkCreateDescriptorPool(device, &descriptor_pool_create, nullptr, &handle);
}

VkResult DescriptorPool::initializeSets() {
	VkResult result;
	result = initializeSetLayouts();
	assert(result == VK_SUCCESS);
	const auto& device = Valkyrie::VulkanManager::getDevice();
	const auto& set_layouts = getSetLayoutHandles();
	VkDescriptorSetAllocateInfo descriptor_set_allocate = {};
	descriptor_set_allocate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_set_allocate.descriptorPool = handle;
	descriptor_set_allocate.descriptorSetCount = sets.size();
	descriptor_set_allocate.pSetLayouts = set_layouts.data();
	result = vkAllocateDescriptorSets(device, &descriptor_set_allocate, sets.data());
	return result;
}

VkResult DescriptorPool::initializeSetLayouts() {
	VkResult result;
	for (auto& set_layout : setLayouts) {
		result = set_layout.initialize();
		if (result != VK_SUCCESS)
			break;
	}
	return result;
}

void DescriptorPool::addPoolSize(VkDescriptorType type, uint32_t count) {
	VkDescriptorPoolSize pool_size = {};
	pool_size.type = type;
	pool_size.descriptorCount = count;
	m_pool_sizes.push_back(pool_size);
}

std::vector<VkDescriptorSetLayout> DescriptorPool::getSetLayoutHandles() {
	std::vector<VkDescriptorSetLayout> handles;
	for (const auto& set_layout : setLayouts) {
		VkDescriptorSetLayout handle = set_layout.handle;
		if (handle == VK_NULL_HANDLE)
			continue;
		handles.push_back(handle);
	}
	return handles;
}

void DescriptorSetLayout::setBinding(const uint32_t shader_binding, const VkDescriptorType type, const VkShaderStageFlags flag, const uint32_t count) {
	VkDescriptorSetLayoutBinding binding = {};
	binding.binding = shader_binding;
	binding.descriptorType = type;
	binding.stageFlags = flag;
	binding.descriptorCount = count;
	binding.pImmutableSamplers = nullptr;
	m_bindings.push_back(binding);
}

VkResult DescriptorSetLayout::initialize() {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create = {};
	descriptor_set_layout_create.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_create.bindingCount = m_bindings.size();
	descriptor_set_layout_create.pBindings = m_bindings.data();
	return vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create, nullptr, &handle);
}

void DescriptorPool::updateDescriptorSet(SetUpdable& update, const uint32_t set_index, const uint32_t binding) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	auto write = update.getWriteSet();
	write.dstSet = sets[set_index];
	write.dstBinding = binding;
	vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void DescriptorPool::updateDescriptorSet(VkWriteDescriptorSet* writes, const uint32_t writes_count, const uint32_t set_index, const uint32_t binding) {
    const auto& device = Valkyrie::VulkanManager::getDevice();
    for (int i = 0; i < writes_count; ++i) {
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = sets[set_index];
        writes[i].dstBinding = binding;
    }
    vkUpdateDescriptorSets(device, writes_count, writes, 0, nullptr);
}
