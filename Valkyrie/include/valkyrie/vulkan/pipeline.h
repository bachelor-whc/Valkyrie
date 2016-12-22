#ifndef _VULKAN_PIPELINE_H
#define _VULKAN_PIPELINE_H
#include "utility.h"
//#include "descriptor_set_layout.h"

namespace Vulkan {
	struct Device;
	struct DescriptorSetLayout;
	struct RenderPass;
	struct VertexInput;

	struct Pipeline {
		static VkPipelineCache cache;
		static VkPipelineLayout layout;
		static VkResult initializeCache(const Device& device);
		static VkResult initializeLayout(const Device& device, const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts);

		Pipeline();
		~Pipeline();

		VkResult initialize(const Device& device);
		void setRenderPass(const RenderPass& render_pass, uint32_t index);
		void setVertexInput(const VertexInput& vertex_input);

		VkPipeline handle = NULL;
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

	typedef std::shared_ptr<Pipeline> PipelinePointer;

	void DestroyPipeline(const Device& device, Pipeline& pipeline);
	void DestroyPipelineCache(const Device& device);
	void DestroyPipelineLayout(const Device& device);
}

#endif