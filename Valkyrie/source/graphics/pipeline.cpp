#include "valkyrie/graphics/pipeline.h"
#include "valkyrie/utility/vulkan_manager.h"
using namespace Valkyrie::Graphics;


Pipeline::Pipeline() :
	module(),
	vertexInput() {

}

Pipeline::~Pipeline() {

}

void Pipeline::initialize() {
	VkResult result;
	for (auto& kv : shaderPtrs) {
		auto shader_ptr = kv.second;
		shader_ptr->initializeModule();
		module.shaderStageCreates.push_back(shader_ptr->createPipelineShaderStage());
	}
	module.setVertexInput(vertexInput);
	result = module.initializeLayout(descriptorPoolPtr->getSetLayoutHandles());
	assert(result == VK_SUCCESS);
	result = module.initialize();
	assert(result == VK_SUCCESS);
}

void Pipeline::commandBind(const Vulkan::CommandBuffer & command) {
	vkCmdBindDescriptorSets(
		command.handle,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		module.layout,
		0,
		descriptorPoolPtr->sets.size(), 
		descriptorPoolPtr->sets.data(),
		0,
		nullptr
	);
	vkCmdBindPipeline(
		command.handle, 
		VK_PIPELINE_BIND_POINT_GRAPHICS, 
		module.handle
	);
}
