#include "valkyrie/vulkan/descriptor.h"
#include "valkyrie/vulkan/device.h"
using namespace Vulkan;

DescriptorPool::DescriptorPool(uint32_t max_sets) : 
	m_max_sets(max_sets),
	m_sizes(),
	m_sets_map(),
	m_set_layouts(),
	m_set_layouts_map() {
	mp_sets = NEW_NT VkDescriptorSet[m_max_sets];
	assert(mp_sets != nullptr);
}

DescriptorPool::~DescriptorPool() {
	for (auto& p_set_layout : m_set_layouts) {
		if (p_set_layout != nullptr)
			delete p_set_layout;
	}
	if (mp_sets != nullptr)
		delete[] mp_sets;
}


VkResult DescriptorPool::initializePool(const Device& device) {
	VkDescriptorPoolCreateInfo descriptor_pool_create = {};
	descriptor_pool_create.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_create.maxSets = m_max_sets;
	descriptor_pool_create.poolSizeCount = (uint32_t)m_sizes.size();
	descriptor_pool_create.pPoolSizes = m_sizes.data();
	return vkCreateDescriptorPool(device.handle, &descriptor_pool_create, nullptr, &handle);
	
}

VkResult DescriptorPool::initializeSets(const Device& device) {
	VkDescriptorSetAllocateInfo descriptor_set_allocate = {};
	std::vector<VkDescriptorSetLayout>& set_layouts = getSetLayoutHandles();
	descriptor_set_allocate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_set_allocate.descriptorPool = handle;
	descriptor_set_allocate.descriptorSetCount = m_sets_size;
	descriptor_set_allocate.pSetLayouts = set_layouts.data();
	VkResult result = vkAllocateDescriptorSets(device.handle, &descriptor_set_allocate, mp_sets);
	return result;
}

VkResult DescriptorPool::initializeSetLayouts(const Device& device) {
	VkResult result;
	for (auto& p_set_layout : m_set_layouts) {
		result = p_set_layout->initialize(device);
		if (result != VK_SUCCESS)
			break;
	}
	return result;
}

VkDescriptorSet& DescriptorPool::registerSet(const std::string& name) {
	if (m_sets_map.find(name) != m_sets_map.end()) {
		return *m_sets_map[name];
	}
	else {
		assert((m_sets_size + 1) <= m_max_sets);
		*(mp_sets + m_sets_size) = VK_NULL_HANDLE;
		m_sets_map[name] = mp_sets + m_sets_size;
		++m_sets_size;
		return *m_sets_map[name];
	}
}

VkDescriptorSet& DescriptorPool::getSet(const std::string& name) {
	return *m_sets_map[name];
}

DescriptorSetLayout& DescriptorPool::registerSetLayout(const std::string& name, const int index) {
	if ((index + 1) > m_set_layouts.size())
		m_set_layouts.resize(index + 1);
	if (m_set_layouts_map.find(name) != m_set_layouts_map.end()) {
		return *m_set_layouts[m_set_layouts_map[name]];
	}
	else {
		DescriptorSetLayout* p_set_layout = NEW_NT DescriptorSetLayout();
		assert(p_set_layout != nullptr);
		m_set_layouts[index] = p_set_layout;
		m_set_layouts_map[name] = index;
		return *p_set_layout;
	}
}

DescriptorSetLayout& DescriptorPool::getSetLayout(const std::string& name) {
	return *m_set_layouts[m_set_layouts_map[name]];
}

void DescriptorPool::addPoolSize(VkDescriptorType type, uint32_t count) {
	VkDescriptorPoolSize pool_size = {};
	pool_size.type = type;
	pool_size.descriptorCount = count;
	m_sizes.push_back(pool_size);
}

std::vector<VkDescriptorSetLayout> DescriptorPool::getSetLayoutHandles() {
	std::vector<VkDescriptorSetLayout> handles;
	for (auto& p_set_layout : m_set_layouts) {
		VkDescriptorSetLayout handle = p_set_layout->handle;
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

VkResult DescriptorSetLayout::initialize(const Device& device) {
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create = {};
	descriptor_set_layout_create.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_create.bindingCount = m_bindings.size();
	descriptor_set_layout_create.pBindings = m_bindings.data();
	return vkCreateDescriptorSetLayout(device.handle, &descriptor_set_layout_create, nullptr, &handle);
}
