#include <iostream>
#include <Valkyrie.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <imgui.h>

const std::string NORMAL_PIPELINE = "NORMAL";
const std::string IMGUI_PIPELINE = "IMGUI";
using namespace Valkyrie;

struct Vertex {
	float position[3];
	float UV[2];
};

struct ModelViewProjection {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};


Vulkan::MemoryTexture CreateImGuiFontsTexture(ValkyrieEngine& valkyrie, ImGuiIO& imgui_io) {
	unsigned char* pixels;
	int texture_width;
	int texture_height;
	imgui_io.Fonts->GetTexDataAsRGBA32(&pixels, &texture_width, &texture_height);

	ImageMemoryPointer p_fonts_image = std::make_shared<RGBA32Memory>(texture_width, texture_height, pixels);
	Vulkan::MemoryTexture fonts_texture(p_fonts_image);
	valkyrie.initailizeTexture(fonts_texture);
	return fonts_texture;
}

int CALLBACK WinMain(HINSTANCE instance_handle, HINSTANCE, LPSTR command_line, int command_show) {
#pragma region INITIALIZE_VALKYRIE
	const int width = 1024;
	const int height = 768;

	Valkyrie::glTFLoader gltfLoader;
	Valkyrie::glTFAssetPtr gltf_ptr = gltfLoader.load("gltf/test.gltf");

	std::string title("Playground");
	ValkyrieEngine valkyrie("Valkyrie");
	valkyrie.initializeWindow(width, height, title);
	valkyrie.initialize();
	auto& imgui_io = ImGui::GetIO();
	imgui_io.DisplaySize.x = width;
	imgui_io.DisplaySize.y = height;
	
#pragma endregion INITIALIZE_VALKYRIE

#pragma region INITIALIZE_VARIABLE
	std::vector<Vertex> vertexs = {
		{{2.0f,2.0f,0.0f},{1.0f,1.0f}},
		{{-2.0f,2.0f,0.0f},{0.0f,1.0f}},
		{{-2.0f,-2.0f,0.0f},{0.0f,0.0f}},
		{{2.0f,-2.0f,0.0f},{1.0f,0.0f}}
	};

	std::vector<uint32_t> indices = { 0,1,2,2,3,0 };

	ModelViewProjection mvp;

	mvp.projection = glm::perspective(60 * 3.14f / 180.0f, (float)width / (float)height, 0.1f, 256.0f);
	mvp.view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -5.0f));

	Vulkan::MemoryBuffer normal_object_buffer;
	Vulkan::MemoryBuffer normal_uniform_buffer;
	Vulkan::MemoryBuffer imgui_vertex_buffer;
	Vulkan::MemoryBuffer imgui_index_buffer;
	ImageFilePointer png_ptr = std::make_shared<STB>();
	Vulkan::ImageTexture texture(png_ptr);
	
	texture.load("wang.png");
	valkyrie.initailizeTexture(texture);

	Vulkan::MemoryTexture memory_texture = CreateImGuiFontsTexture(valkyrie, imgui_io);
	imgui_io.Fonts->TexID = (void*)memory_texture.image;

	normal_object_buffer.allocate(
		{ VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT },
		{ (uint32_t)(vertexs.size() * sizeof(Vertex)), (uint32_t)(indices.size() * sizeof(uint32_t)) });
	normal_uniform_buffer.allocate({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT }, { sizeof(mvp) });

	normal_object_buffer.write(vertexs.data(), 0);
	normal_object_buffer.write(indices.data(), 1);
	normal_uniform_buffer.write(&mvp, 0);

#pragma endregion INITIALIZE_VARIABLE

	auto& nomal_set_layout = valkyrie.descriptorPool.registerSetLayout(NORMAL_PIPELINE, 0);
	auto& imgui_set_layout = valkyrie.descriptorPool.registerSetLayout(IMGUI_PIPELINE, 1);

	nomal_set_layout.setBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);
	nomal_set_layout.setBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	imgui_set_layout.setBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	valkyrie.initializeDescriptorSetLayouts();

	valkyrie.createPipelineModule(NORMAL_PIPELINE);
	valkyrie.createPipelineModule(IMGUI_PIPELINE);
	
	auto p_normal_pipeline = valkyrie.pipelines[NORMAL_PIPELINE];
	auto p_imgui_pipeline = valkyrie.pipelines[IMGUI_PIPELINE];

	valkyrie.vertexInputs[NORMAL_PIPELINE]->setBindingDescription(0, sizeof(Vertex));
	valkyrie.vertexInputs[NORMAL_PIPELINE]->setAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
	valkyrie.vertexInputs[NORMAL_PIPELINE]->setAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, 3 * sizeof(float));
	valkyrie.vertexInputs[IMGUI_PIPELINE]->setBindingDescription(0, sizeof(ImDrawVert));
	valkyrie.vertexInputs[IMGUI_PIPELINE]->setAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, 0);
	valkyrie.vertexInputs[IMGUI_PIPELINE]->setAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, sizeof(ImVec2));
	valkyrie.vertexInputs[IMGUI_PIPELINE]->setAttributeDescription(0, 2, VK_FORMAT_R8G8B8A8_UNORM, 2 * sizeof(ImVec2));

	VkPushConstantRange imgui_push_constant = {};
	imgui_push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	imgui_push_constant.size = sizeof(float) * 4;
	p_imgui_pipeline->pushConstantRanges.push_back(imgui_push_constant);

	std::string normal_vertex_code = Vulkan::Shader::LoadSPVBinaryCode("shader.vert.spv");
	std::string normal_fragment_code = Vulkan::Shader::LoadSPVBinaryCode("shader.frag.spv");
	std::string imgui_vertex_code = Vulkan::Shader::LoadSPVBinaryCode("imgui.vert.spv");
	std::string imgui_fragment_code = Vulkan::Shader::LoadSPVBinaryCode("imgui.frag.spv");

	valkyrie.shaders["VERTEX"] = std::make_shared<Vulkan::Shader>(normal_vertex_code, VK_SHADER_STAGE_VERTEX_BIT);
	valkyrie.shaders["FRAGMENT"] = std::make_shared<Vulkan::Shader>(normal_fragment_code, VK_SHADER_STAGE_FRAGMENT_BIT);
	valkyrie.shaders["IMGUI_VERTEX"] = std::make_shared<Vulkan::Shader>(imgui_vertex_code, VK_SHADER_STAGE_VERTEX_BIT);
	valkyrie.shaders["IMGUI_FRAGMENT"] = std::make_shared<Vulkan::Shader>(imgui_fragment_code, VK_SHADER_STAGE_FRAGMENT_BIT);

	valkyrie.initializeShaderModules();

	p_normal_pipeline->shaderStageCreates.push_back(valkyrie.shaders["VERTEX"]->createPipelineShaderStage());
	p_normal_pipeline->shaderStageCreates.push_back(valkyrie.shaders["FRAGMENT"]->createPipelineShaderStage());
	p_imgui_pipeline->shaderStageCreates.push_back(valkyrie.shaders["IMGUI_VERTEX"]->createPipelineShaderStage());
	p_imgui_pipeline->shaderStageCreates.push_back(valkyrie.shaders["IMGUI_FRAGMENT"]->createPipelineShaderStage());

	valkyrie.initializePipelineLayout(NORMAL_PIPELINE);
	valkyrie.initializePipelineLayout(IMGUI_PIPELINE);
	valkyrie.initializePipeline(NORMAL_PIPELINE);
	valkyrie.initializePipeline(IMGUI_PIPELINE);
	valkyrie.descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
	valkyrie.descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	valkyrie.descriptorPool.registerSet(IMGUI_PIPELINE, 1);
	valkyrie.descriptorPool.registerSet(NORMAL_PIPELINE, 0);
	valkyrie.initializeDescriptorPool();
	valkyrie.initializeDescriptorSets();

	std::vector<VkWriteDescriptorSet> writes(3);
	VkWriteDescriptorSet& shader_uniform = writes[0];
	VkWriteDescriptorSet& shader_samplers = writes[1];
	VkWriteDescriptorSet& imgui_samplers = writes[2];

	shader_uniform = {};
	shader_uniform.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	shader_uniform.dstSet = valkyrie.descriptorPool.getSet(NORMAL_PIPELINE);
	shader_uniform.descriptorCount = 1;
	shader_uniform.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	shader_uniform.pBufferInfo = normal_uniform_buffer.getInformationPointer(0);
	shader_uniform.dstBinding = 0;

	shader_samplers = {};
	shader_samplers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	shader_samplers.dstSet = valkyrie.descriptorPool.getSet(NORMAL_PIPELINE);
	shader_samplers.descriptorCount = 1;
	shader_samplers.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	shader_samplers.pImageInfo = texture.getInformationPointer();
	shader_samplers.dstBinding = 1;

	imgui_samplers = {};
	imgui_samplers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	imgui_samplers.dstSet = valkyrie.descriptorPool.getSet(IMGUI_PIPELINE);
	imgui_samplers.descriptorCount = 1;
	imgui_samplers.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	imgui_samplers.pImageInfo = memory_texture.getInformationPointer();
	imgui_samplers.dstBinding = 0;

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

	float scale[2];
	scale[0] = 2.0f / imgui_io.DisplaySize.x;
	scale[1] = 2.0f / imgui_io.DisplaySize.y;
	float translate[2];
	translate[0] = -1.0f;
	translate[1] = -1.0f;
	
	VkResult result;
	int render_command_size = valkyrie.renderCommands.size();
	Vulkan::SecondaryCommandBuffers& secondary_buffers = valkyrie.createSecondaryCommandBuffers(2);
	for (int i = 0; i < render_command_size; ++i) {
		auto& command = valkyrie.renderCommands[i];
		valkyrie.renderCommands[i].begin();
		render_pass_begin.framebuffer = valkyrie.getFramebuffer(i);
		vkCmdBeginRenderPass(command.handle, &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

		valkyrie.commandSetViewport(command);
		valkyrie.commandSetScissor(command);

		const VkDeviceSize normal_offsets[1] = { normal_object_buffer.getOffset(0) };
		
		vkCmdBindDescriptorSets(
			command.handle, 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			p_normal_pipeline->layout, 
			0, 
			valkyrie.descriptorPool.getSetsSize(), valkyrie.descriptorPool.getSets(), 
			0, nullptr
		);
		vkCmdBindPipeline(command.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, p_normal_pipeline->handle);
		vkCmdBindVertexBuffers(command.handle, 0, 1, &normal_object_buffer.handle, normal_offsets);
		vkCmdBindIndexBuffer(command.handle, normal_object_buffer.handle, normal_object_buffer.getOffset(1), VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(command.handle, indices.size(), 1, 0, 0, 1);
		
		vkCmdExecuteCommands(command.handle, 1, secondary_buffers.data() + i);

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
	std::vector<void*> parameter({ &valkyrie, &p_normal_pipeline });
	const VkDeviceSize imgui_offsets[1] = { 0 };

	bool test_window = true;

	int count = 0;
	while (valkyrie.execute()) {
		rotation.x += 0.01f;
		rotation.y += 0.01f;
		mvp.model = glm::mat4();
		mvp.model = glm::rotate(mvp.model, rotation.x * 3.14f / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		mvp.model = glm::rotate(mvp.model, rotation.y * 3.14f / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mvp.model = glm::rotate(mvp.model, rotation.z * 3.14f / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		normal_uniform_buffer.write(&mvp, 0);

		const auto& mouse_pos = imgui_io.MousePos;
		ImGui::NewFrame();
		ImGui::Text(std::to_string(count++).c_str());
		ImGui::Text((std::to_string(mouse_pos.x) + ", " + std::to_string(mouse_pos.y)).c_str());
		ImGui::Text(std::to_string(valkyrie.getFPS()).c_str());
		ImGui::ShowTestWindow(&test_window);
		ImGui::Render();
		auto draw_data = ImGui::GetDrawData();

		uint32_t total_vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
		imgui_vertex_buffer.allocate(
		{ VK_BUFFER_USAGE_VERTEX_BUFFER_BIT },
		{ total_vertex_size });

		uint32_t total_index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
		imgui_index_buffer.allocate(
		{ VK_BUFFER_USAGE_INDEX_BUFFER_BIT },
		{ total_index_size });

		ImDrawVert* vertex_memory_destination = (ImDrawVert*)imgui_vertex_buffer.startWriting(0);
		ImDrawIdx* index_memory_destination = (ImDrawIdx*)imgui_index_buffer.startWriting(0);

		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const auto command_list = draw_data->CmdLists[n];
			memcpy(vertex_memory_destination, command_list->VtxBuffer.Data, command_list->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(index_memory_destination, command_list->IdxBuffer.Data, command_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			vertex_memory_destination += command_list->VtxBuffer.Size;
			index_memory_destination += command_list->IdxBuffer.Size;
		}
		imgui_vertex_buffer.endWriting();
		imgui_index_buffer.endWriting();

		VkCommandBufferInheritanceInfo command_buffer_inheritance = {};
		command_buffer_inheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		command_buffer_inheritance.pNext = NULL;
		command_buffer_inheritance.renderPass = valkyrie.getRenderPassHandle();
		command_buffer_inheritance.subpass = 0;
		command_buffer_inheritance.framebuffer = valkyrie.getFramebuffer(valkyrie.getCurrentBuffer());
		command_buffer_inheritance.occlusionQueryEnable = VK_FALSE;
		command_buffer_inheritance.queryFlags = 0;
		command_buffer_inheritance.pipelineStatistics = 0;

		VkCommandBufferBeginInfo secondary_begin = {};
		secondary_begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		secondary_begin.pNext = NULL;
		secondary_begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT |
			VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		secondary_begin.pInheritanceInfo = &command_buffer_inheritance;

		VkCommandBuffer command = secondary_buffers[valkyrie.getCurrentBuffer()];

		vkBeginCommandBuffer(command, &secondary_begin);
		vkCmdBindDescriptorSets(
			command,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			p_imgui_pipeline->layout,
			0,
			valkyrie.descriptorPool.getSetsSize(), valkyrie.descriptorPool.getSets(),
			0, nullptr
		);
		vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, p_imgui_pipeline->handle);

		vkCmdPushConstants(command, p_imgui_pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 0, sizeof(float) * 2, scale);
		vkCmdPushConstants(command, p_imgui_pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 2, sizeof(float) * 2, translate);

		vkCmdBindVertexBuffers(command, 0, 1, &imgui_vertex_buffer.handle, imgui_offsets);
		vkCmdBindIndexBuffer(command, imgui_index_buffer.handle, 0, VK_INDEX_TYPE_UINT16);

		int vertex_offset = 0;
		int index_offset = 0;
		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const ImDrawList* command_list = draw_data->CmdLists[n];
			for (int cmd_i = 0; cmd_i < command_list->CmdBuffer.Size; cmd_i++) {
				const ImDrawCmd* p_command = &command_list->CmdBuffer[cmd_i];
				if (p_command->UserCallback) {
					p_command->UserCallback(command_list, p_command);
				}
				else {
					vkCmdDrawIndexed(command, p_command->ElemCount, 1, index_offset, vertex_offset, 0);
				}
				index_offset += p_command->ElemCount;
			}
			vertex_offset += command_list->VtxBuffer.Size;
		}
		vkEndCommandBuffer(command);
	}
	return 0;
}