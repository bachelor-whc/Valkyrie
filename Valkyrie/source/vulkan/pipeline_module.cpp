#include "valkyrie/vulkan/pipeline_module.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/descriptor.h"
#include "valkyrie/vulkan/render_pass.h"
#include "valkyrie/vulkan/vertex_input.h"
#include "valkyrie/utility/vulkan_manager.h"
using namespace Vulkan;

VkPipelineCache PipelineModule::cache = VK_NULL_HANDLE;

VkResult PipelineModule::initializeCache() {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	VkPipelineCacheCreateInfo pipeline_cache_create = {};
	pipeline_cache_create.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	return vkCreatePipelineCache(device, &pipeline_cache_create, NULL, &cache);
}

VkResult PipelineModule::initializeLayout(const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	size_t layout_count = descriptor_set_layouts.size();
	VkPipelineLayoutCreateInfo pipeline_layout_create = {};
	pipeline_layout_create.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create.setLayoutCount = (uint32_t)layout_count;
	pipeline_layout_create.pSetLayouts = descriptor_set_layouts.data();
	pipeline_layout_create.pushConstantRangeCount = pushConstantRanges.size();
	pipeline_layout_create.pPushConstantRanges = pushConstantRanges.data();
	VkResult result = vkCreatePipelineLayout(device, &pipeline_layout_create, nullptr, &layout);
	return result;
}

PipelineModule::PipelineModule() :
	shaderStageCreates(),
	pushConstantRanges() {
	

	m_pipeline_create.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	m_pipeline_create.pVertexInputState = &m_vertex_input_state;
	m_pipeline_create.pInputAssemblyState = &m_input_assembly_state;
	m_pipeline_create.pTessellationState = nullptr;
	m_pipeline_create.pViewportState = &m_viewport_state;
	m_pipeline_create.pRasterizationState = &m_rasterization_state;
	m_pipeline_create.pMultisampleState = &m_multisample_state;
	m_pipeline_create.pDepthStencilState = &m_depth_state;
	m_pipeline_create.pColorBlendState = &m_color_blend_state;
	m_pipeline_create.pDynamicState = &m_dynamic_state;
	colorBlendAttachments.resize(1);
	colorBlendAttachments[0] = VK_DEFAULT_COLOR_BLEND_ATTACHMENT_STATE;
}

PipelineModule::~PipelineModule() {

}

VkResult PipelineModule::initialize() {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	assert(cache != VK_NULL_HANDLE);
	initializeVertexInputState();
	initializeInputAssemblyState();
	initializeViewportState();
	initializeRasterizationState();
	initializeMultisampleState();
	initializeDepthStencilState();
	initializeColorBlendState();
	initializeDynamicState();
	m_pipeline_create.stageCount = (uint32_t)shaderStageCreates.size();
	m_pipeline_create.pStages = shaderStageCreates.data();
	m_pipeline_create.layout = layout;
	return vkCreateGraphicsPipelines(device, cache, 1, &m_pipeline_create, nullptr, &handle);
}

void PipelineModule::setRenderPass(const RenderPass& render_pass, uint32_t index) {
	m_pipeline_create.renderPass = render_pass.handle;
	m_pipeline_create.subpass = index;
}

void PipelineModule::setVertexInput(const VertexInput& vertex_input) {
	m_vertex_input_state.vertexBindingDescriptionCount = (uint32_t)vertex_input.bindings.size();
	m_vertex_input_state.pVertexBindingDescriptions = vertex_input.bindings.data();
	m_vertex_input_state.vertexAttributeDescriptionCount = (uint32_t)vertex_input.attributes.size();
	m_vertex_input_state.pVertexAttributeDescriptions = vertex_input.attributes.data();
}

void PipelineModule::initializeVertexInputState() {
	m_vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
}

void PipelineModule::initializeInputAssemblyState() {
	m_input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_input_assembly_state.primitiveRestartEnable = VK_FALSE;
	m_input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void PipelineModule::initializeViewportState() {
	m_viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_viewport_state.viewportCount = 1;
	m_viewport_state.scissorCount = 1;
}

void PipelineModule::initializeRasterizationState() {
	m_rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
	m_rasterization_state.cullMode = VK_CULL_MODE_NONE;
	m_rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	m_rasterization_state.depthClampEnable = VK_FALSE;
	m_rasterization_state.rasterizerDiscardEnable = VK_FALSE;
	m_rasterization_state.depthBiasEnable = VK_FALSE;
	m_rasterization_state.lineWidth = 1.0f;
}

void PipelineModule::initializeMultisampleState() {
	m_multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	m_multisample_state.sampleShadingEnable = VK_FALSE;
	m_multisample_state.alphaToCoverageEnable = VK_FALSE;
	m_multisample_state.alphaToOneEnable = VK_FALSE;
	m_multisample_state.minSampleShading = 0.0;
}

void PipelineModule::initializeDepthStencilState() {
	m_depth_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_depth_state.depthTestEnable = VK_TRUE;
	m_depth_state.depthWriteEnable = VK_TRUE;
	m_depth_state.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	m_depth_state.depthBoundsTestEnable = VK_FALSE;
	m_depth_state.stencilTestEnable = VK_FALSE;
	m_depth_state.back.failOp = VK_STENCIL_OP_KEEP;
	m_depth_state.back.passOp = VK_STENCIL_OP_KEEP;
	m_depth_state.back.compareOp = VK_COMPARE_OP_ALWAYS;
	m_depth_state.back.depthFailOp = VK_STENCIL_OP_KEEP;
	m_depth_state.front = m_depth_state.back;
}

void PipelineModule::initializeColorBlendState() {
	m_color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_color_blend_state.attachmentCount = (uint32_t)colorBlendAttachments.size();
	m_color_blend_state.pAttachments = colorBlendAttachments.data();
	m_color_blend_state.blendConstants[0] = 0.0f;
	m_color_blend_state.blendConstants[1] = 0.0f;
	m_color_blend_state.blendConstants[2] = 0.0f;
	m_color_blend_state.blendConstants[3] = 0.0f;
}

void PipelineModule::initializeDynamicState() {
	m_dynamic_state_enables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	m_dynamic_state_enables.push_back(VK_DYNAMIC_STATE_SCISSOR);
	m_dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	m_dynamic_state.pDynamicStates = m_dynamic_state_enables.data();
	m_dynamic_state.dynamicStateCount = (uint32_t)m_dynamic_state_enables.size();
}

void Vulkan::DestroyPipeline(PipelineModule& pipeline) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	vkDestroyPipelineLayout(device, pipeline.layout, nullptr);
	vkDestroyPipeline(device, pipeline.handle, nullptr);
}

void Vulkan::DestroyPipelineCache() {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	vkDestroyPipelineCache(device, PipelineModule::cache, nullptr);
}
