#include "valkyrie/vulkan/shader.h"
#include "valkyrie/vulkan/device.h"
using namespace Vulkan;

std::string Shader::LoadSPVBinaryCode(const std::string file_path) {
	std::ifstream spv(file_path, std::ios::in | std::ios::binary);
	assert(spv.is_open() && spv.is_open());
	std::string code((std::istreambuf_iterator<char>(spv)), std::istreambuf_iterator<char>());
	spv.close();
	return code;
}

Shader::Shader(const std::string spv_code, const VkShaderStageFlagBits flag, std::string entry) :
	m_binary_code(spv_code),
	m_flag(flag),
	m_entry_point(entry) {

}

Shader::~Shader() {

}

VkResult Shader::initializeModule() {
	VkShaderModuleCreateInfo shader_module_create = {};
	shader_module_create.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_create.codeSize = m_binary_code.size();
	shader_module_create.pCode = (uint32_t*)m_binary_code.c_str();
	return vkCreateShaderModule(g_device_handle, &shader_module_create, nullptr, &handle);
}

VkPipelineShaderStageCreateInfo Shader::createPipelineShaderStage() {
	assert(handle != NULL);
	VkPipelineShaderStageCreateInfo shader_stage_create = {};
	shader_stage_create.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_create.stage = m_flag;
	shader_stage_create.pName = m_entry_point.c_str();
	shader_stage_create.module = handle;
	return shader_stage_create;
}