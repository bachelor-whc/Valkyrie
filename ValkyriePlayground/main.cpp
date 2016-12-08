#include <iostream>
#include <Valkyrie.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <cassert>

struct Vertex {
	float position[3];
	//float color[3];
	float UV[2];
};

struct ModelViewProjection {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

int CALLBACK WinMain(HINSTANCE instance_handle, HINSTANCE, LPSTR command_line, int command_show) {
	const int width = 800;
	const int height = 600;

	Wendy::Win32Window window(width, height, instance_handle);
	std::string title("Playground");
	window.create(title);
	Valkyrie valkyrie("Valkyrie");
	valkyrie.setWindowPointer(&window);
	valkyrie.initialize();
	valkyrie.vertexInput.setBindingDescription(0, sizeof(Vertex));
	valkyrie.vertexInput.setAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
	valkyrie.vertexInput.setAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, 3 * sizeof(float));

	std::vector<Vertex> vertexs = {
		{{1.0f,1.0f,0.0f},{1.0f,1.0f}},
		{{-1.0f,1.0f,0.0f},{0.0f,1.0f}},
		{{-1.0f,-1.0f,0.0f},{0.0f,0.0f}},
		{{1.0f,-1.0f,0.0f},{1.0f,0.0f}}
	};

	std::vector<uint32_t> indices = {0,1,2,2,3,0};

	ModelViewProjection mvp;

	mvp.projection = glm::perspective(60 * 3.14f / 180.0f, (float)width / (float)height, 0.1f, 256.0f);
	mvp.view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -5.0f));

	Vulkan::MemoryBuffer vertex_buffer;
	Vulkan::MemoryBuffer index_buffer;
	Vulkan::MemoryBuffer uniform_buffer;
	ValkyriePNGPointer png_ptr = std::make_shared<ValkyriePNG>();
	Vulkan::Texture texture(png_ptr);
	
	valkyrie.allocateMemoryBuffer(vertex_buffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexs.size() * sizeof(Vertex));
	valkyrie.allocateMemoryBuffer(index_buffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices.size() * sizeof(uint32_t));
	valkyrie.allocateMemoryBuffer(uniform_buffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(mvp));
	valkyrie.writeMemoryBuffer(vertex_buffer, vertexs.data());
	valkyrie.writeMemoryBuffer(index_buffer, indices.data());
	valkyrie.writeMemoryBuffer(uniform_buffer, &mvp);
	texture.load("wang.png");
	valkyrie.initailizeTexture(texture);
	
	valkyrie.descriptorPool.setLayout.setBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);
	valkyrie.descriptorPool.setLayout.setBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	valkyrie.initializeDescriptorSetLayout();
	
	valkyrie.initializePipelineLayout();
	valkyrie.pipelines["NORMAL"] = std::make_shared<Vulkan::Pipeline>();
	auto p_pipeline = valkyrie.pipelines["NORMAL"];

	std::string vertex_code = Vulkan::Shader::LoadSPVBinaryCode("vert.spv");
	std::string fragment_code = Vulkan::Shader::LoadSPVBinaryCode("frag.spv");
	
	valkyrie.shaders["VERTEX"] = std::make_shared<Vulkan::Shader>(vertex_code, VK_SHADER_STAGE_VERTEX_BIT);
	valkyrie.shaders["FRAGMENT"] = std::make_shared<Vulkan::Shader>(fragment_code, VK_SHADER_STAGE_FRAGMENT_BIT);

	valkyrie.initializeShaderModules();

	p_pipeline->shaderStageCreates.push_back(valkyrie.shaders["VERTEX"]->createPipelineShaderStage());
	p_pipeline->shaderStageCreates.push_back(valkyrie.shaders["FRAGMENT"]->createPipelineShaderStage());

	valkyrie.initializePipelines();
	valkyrie.descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
	valkyrie.descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	valkyrie.initializeDescriptorPool();
	valkyrie.initializeDescriptorSets();

	std::vector<VkWriteDescriptorSet> writes(2);
	VkWriteDescriptorSet& write_uniform = writes[0];
	VkWriteDescriptorSet& write_sampler = writes[1];

	write_uniform = {};
	write_uniform.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write_uniform.dstSet = valkyrie.descriptorPool.set;
	write_uniform.descriptorCount = 1;
	write_uniform.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write_uniform.pBufferInfo = uniform_buffer.getInformationPointer();
	write_uniform.dstBinding = 0;

	write_sampler = {};
	write_sampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write_sampler.dstSet = valkyrie.descriptorPool.set;
	write_sampler.descriptorCount = 1;
	write_sampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write_sampler.pImageInfo = texture.getInformationPointer();
	write_sampler.dstBinding = 1;

	valkyrie.writeSets(writes);

	VkClearValue clear_values[2];
	clear_values[0].color.float32[0] = 0.2f;
	clear_values[0].color.float32[1] = 0.2f;
	clear_values[0].color.float32[2] = 0.2f;
	clear_values[0].color.float32[3] = 0.2f;
	clear_values[1].depthStencil.depth = 1.0f;
	clear_values[1].depthStencil.stencil = 0;

	VkRenderPassBeginInfo render_pass_begin;
	render_pass_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin.pNext = NULL;
	render_pass_begin.renderPass = valkyrie.getRenderPassHandle();
	render_pass_begin.renderArea.offset.x = 0;
	render_pass_begin.renderArea.offset.y = 0;
	render_pass_begin.renderArea.extent.width = width;
	render_pass_begin.renderArea.extent.height = height;
	render_pass_begin.clearValueCount = 2;
	render_pass_begin.pClearValues = clear_values;

	VkResult result;
	int render_command_size = valkyrie.renderCommands.size();
	for (int i = 0; i < render_command_size; ++i) {
		auto& command = valkyrie.renderCommands[i];
		valkyrie.renderCommands[i].begin();
		render_pass_begin.framebuffer = valkyrie.getFramebuffer(i);
		vkCmdBeginRenderPass(command.handle, &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

		valkyrie.commandSetViewport(command);
		valkyrie.commandSetScissor(command);

		vkCmdBindDescriptorSets(command.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, p_pipeline->layout, 0, 1, &valkyrie.descriptorPool.set, 0, nullptr);
		vkCmdBindPipeline(command.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, p_pipeline->handle);

		const VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(command.handle, 0, 1, &vertex_buffer.handle, offsets);
		vkCmdBindIndexBuffer(command.handle, index_buffer.handle, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(command.handle, indices.size(), 1, 0, 0, 1);
		vkCmdEndRenderPass(command.handle);

		VkImageMemoryBarrier previous_present_barrier = {};
		previous_present_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		previous_present_barrier.pNext = NULL;
		previous_present_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		previous_present_barrier.dstAccessMask = 0;
		previous_present_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		previous_present_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		previous_present_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		previous_present_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		previous_present_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		previous_present_barrier.subresourceRange.baseMipLevel = 0;
		previous_present_barrier.subresourceRange.levelCount = 1;
		previous_present_barrier.subresourceRange.baseArrayLayer = 0;
		previous_present_barrier.subresourceRange.layerCount = 1;
		previous_present_barrier.image = valkyrie.getSwapChainImage(i);
		vkCmdPipelineBarrier(command.handle, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &previous_present_barrier);
		result = command.end();
		assert(result == VK_SUCCESS);
	}

	glm::vec3 rotation = glm::vec3();
	
	while (valkyrie.execute()) {
		rotation.x += 0.01f;
		rotation.y += 0.01f;
		mvp.model = glm::mat4();
		mvp.model = glm::rotate(mvp.model, rotation.x * 3.14f / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		mvp.model = glm::rotate(mvp.model, rotation.y * 3.14f / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mvp.model = glm::rotate(mvp.model, rotation.z * 3.14f / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		valkyrie.writeMemoryBuffer(uniform_buffer, &mvp);
	}
	return 0;
}