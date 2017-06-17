#ifndef _VULKAN_SHADER_H
#define _VULKAN_SHADER_H
#include "valkyrie/common.h"

namespace Vulkan {
	struct Device;

	struct Shader {
		static std::string LoadSPVBinaryCode(const std::string file_path);

		Shader() = delete;
		Shader(const std::string spv_code, const VkShaderStageFlagBits flag, std::string entry = "main");
		~Shader();

		VkResult initializeModule();
		VkPipelineShaderStageCreateInfo createPipelineShaderStage();

		VkShaderModule handle = NULL;
	private:
		std::string m_binary_code;
		VkShaderStageFlagBits m_flag;
		std::string m_entry_point;
	};

	typedef std::shared_ptr<Shader> ShaderPointer;
}

#endif