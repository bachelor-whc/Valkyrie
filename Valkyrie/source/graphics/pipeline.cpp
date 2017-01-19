#include "valkyrie/graphics/pipeline.h"
#include "valkyrie/utility/vulkan_manager.h"
using namespace Valkyrie::Graphics;


Pipeline::Pipeline() :
	module(),
	vertexInput() {

}

Pipeline::~Pipeline() {

}

void Pipeline::initializePipeline() {
	VkResult result;
	module.setVertexInput(vertexInput);
	result = module.initialize();
	assert(result == VK_SUCCESS);
}
