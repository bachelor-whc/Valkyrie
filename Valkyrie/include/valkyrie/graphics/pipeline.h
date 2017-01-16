#ifndef _VALKYRIE_GRAPHICS_PIPELINE_H
#define _VALKYRIE_GRAPHICS_PIPELINE_H
#include "valkyrie/vulkan/pipeline_module.h"
#include "valkyrie/vulkan/vertex_input.h"
#include "valkyrie/vulkan/shader.h"
#include "valkyrie/vulkan/descriptor.h"
#include "common.h"

namespace Valkyrie {
	namespace Graphics {
		class Pipeline {
		public:
			enum ShaderStage {
				VERTEX,
				FRAGMENT
			};

			Pipeline();
			virtual ~Pipeline();

			void writeSets(const std::vector<VkWriteDescriptorSet>& writes);
			void initializePipeline();
			void initializeDescriptorPool();
			void initializeDescriptorSets();
			void initializeShaderModules();
			void initializePipelineLayout(const std::string& pipeline_name);
			void initializeDescriptorSetLayouts();

			Vulkan::VertexInput vertexInput;
			Vulkan::DescriptorPool descriptorPool;

		private:
			Vulkan::PipelineModule module;
			std::map<ShaderStage, Vulkan::ShaderPointer> shaders;
		};
	}
}

#endif