#ifndef _VULKAN_PIPELINE_H
#define _VULKAN_PIPELINE_H
#include "utility.h"

namespace Vulkan {
	struct Device;
	struct DescriptorSetLayout;
	struct RenderPass;
	struct VertexInput;

	struct PipelineModule {
		static VkPipelineCache cache;
		static VkResult initializeCache(const Device& device);
		VkResult initializeLayout(const Device& device, const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts);

		PipelineModule();
		virtual ~PipelineModule();

		VkResult initialize(const Device& device);
		void setRenderPass(const RenderPass& render_pass, uint32_t index);
		void setVertexInput(const VertexInput& vertex_input);

		VkPipeline handle = VK_NULL_HANDLE;
		VkPipelineLayout layout = VK_NULL_HANDLE;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreates;

	private:
		void initializeVertexInputState();
		void initializeInputAssemblyState();
		//void initializeTessellationState();
		void initializeViewportState();
		void initializeRasterizationState();
		void initializeMultisampleState();
		void initializeDepthStencilState();
		void initializeColorBlendState();
		void initializeDynamicState();


		VkGraphicsPipelineCreateInfo m_pipeline_create = {};

		VkPipelineVertexInputStateCreateInfo m_vertex_input_state = {};
		VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state = {};
		//VkPipelineTessellationStateCreateInfo m_tessellation_state = {};
		VkPipelineViewportStateCreateInfo m_viewport_state = {};
		VkPipelineRasterizationStateCreateInfo m_rasterization_state = {};
		VkPipelineMultisampleStateCreateInfo m_multisample_state = {};
		VkPipelineDepthStencilStateCreateInfo m_depth_state = {};
		VkPipelineColorBlendStateCreateInfo m_color_blend_state = {};
		std::vector<VkPipelineColorBlendAttachmentState> m_color_blend_attachments;
		VkPipelineDynamicStateCreateInfo m_dynamic_state = {};
		std::vector<VkDynamicState> m_dynamic_state_enables;
	};

	typedef std::shared_ptr<PipelineModule> PipelinePointer;

	void DestroyPipeline(const Device& device, PipelineModule& pipeline);
	void DestroyPipelineCache(const Device& device);
}

#endif