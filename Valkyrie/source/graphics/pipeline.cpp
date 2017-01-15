#include "valkyrie/graphics/pipeline.h"
using namespace Valkyrie::Graphics;


Pipeline::Pipeline() :
	module(),
	vertexInput(),
	descriptorPool(8) {

}

Pipeline::~Pipeline() {

}

void Pipeline::writeSets(const std::vector<VkWriteDescriptorSet>& writes) {
	vkUpdateDescriptorSets(g_device_handle, writes.size(), writes.data(), 0, NULL);
}

void Pipeline::initializePipeline() {
	VkResult result;
	module.setVertexInput(vertexInput);
	result = module.initialize();
	assert(result == VK_SUCCESS);
}

void Pipeline::initializeDescriptorPool() {
	VkResult result = descriptorPool.initializePool();
	assert(result == VK_SUCCESS);
}

void Pipeline::initializeDescriptorSets() {
	VkResult result;
	result = descriptorPool.initializeSets();
	assert(result == VK_SUCCESS);

}

void Pipeline::initializeShaderModules() {
	VkResult result;
	for (auto& key_value : shaders) {
		auto& shader_ptr = key_value.second;
		result = shader_ptr->initializeModule();
		assert(result == VK_SUCCESS);
	}
}

void Pipeline::initializePipelineLayout(const std::string & pipeline_name) {
	VkResult result;
	std::vector<VkDescriptorSetLayout>& set_layouts = descriptorPool.getSetLayoutHandles();
	result = module.initializeLayout(set_layouts);
	assert(result == VK_SUCCESS);
}

void Pipeline::initializeDescriptorSetLayouts() {
	VkResult result;
	result = descriptorPool.initializeSetLayouts();
	assert(result == VK_SUCCESS);
}