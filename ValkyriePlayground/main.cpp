#include <iostream>
#include <Valkyrie.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <imgui.h>
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 

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

class ImGuiRenderFunction : public ValkyrieRenderFunction {
public:
	ImGuiRenderFunction() :
		ValkyrieRenderFunction(),
		mp_vertex_buffer(nullptr),
		mp_index_buffer(nullptr),
		m_command_buffer() {
		mp_vertex_buffer = NEW_NT Vulkan::MemoryBuffer[VALKYRIE_FRAME_BUFFER_COUNT];
		mp_index_buffer = NEW_NT Vulkan::MemoryBuffer[VALKYRIE_FRAME_BUFFER_COUNT];
		assert(mp_vertex_buffer != nullptr);
		assert(mp_index_buffer != nullptr);
	}

	~ImGuiRenderFunction() {
		if (mp_vertex_buffer == nullptr)
			delete[] mp_vertex_buffer;
		if (mp_index_buffer == nullptr)
			delete[] mp_index_buffer;
	}

private:
	virtual void implement(const std::vector<void*>& data, uint32_t current_buffer) override {
		ImGui::NewFrame();
		ImGui::Text("Hello");
		ImGui::RadioButton("XXXX", true);
		bool y = true;
		ImGui::ShowTestWindow(&y);
		ImGui::Render();
		auto draw_data = ImGui::GetDrawData();
		ImGuiIO& imgui_io = ImGui::GetIO();

		auto& vertex_buffer = mp_vertex_buffer[current_buffer];
		auto& index_buffer = mp_index_buffer[current_buffer];
		Valkyrie& valkyrie = *((Valkyrie*)data[0]);
		Vulkan::PipelinePointer& p_pipeline = *((Vulkan::PipelinePointer*)data[1]);

		size_t total_vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
		if (vertex_buffer.handle == VK_NULL_HANDLE || vertex_buffer.getSize() < total_vertex_size) {
			if (vertex_buffer.handle != VK_NULL_HANDLE || vertex_buffer.memory != VK_NULL_HANDLE)
				valkyrie.destroyMemoryBuffer(vertex_buffer);
			valkyrie.allocateMemoryBuffer(vertex_buffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, total_vertex_size);
		}

		size_t total_index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
		if (index_buffer.handle == VK_NULL_HANDLE || index_buffer.getSize() < total_index_size) {
			if (index_buffer.handle != VK_NULL_HANDLE || index_buffer.memory != VK_NULL_HANDLE)
				valkyrie.destroyMemoryBuffer(index_buffer);
			valkyrie.allocateMemoryBuffer(index_buffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, total_index_size);
		}

		ImDrawVert* vertex_memory_destination = (ImDrawVert*)valkyrie.startWritingMemoryBuffer(vertex_buffer);
		ImDrawIdx* index_memory_destination = (ImDrawIdx*)valkyrie.startWritingMemoryBuffer(index_buffer);
		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const auto command_list = draw_data->CmdLists[n];
			memcpy(vertex_memory_destination, command_list->VtxBuffer.Data, command_list->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(index_memory_destination, command_list->IdxBuffer.Data, command_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			vertex_memory_destination += command_list->VtxBuffer.Size;
			index_memory_destination += command_list->IdxBuffer.Size;
		}

		if (m_command_buffer.handle == VK_NULL_HANDLE)
			m_command_buffer = valkyrie.createCommandBuffer();

		vkCmdBindPipeline(m_command_buffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, p_pipeline->handle);
		vkCmdBindDescriptorSets(m_command_buffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, p_pipeline->layout, 0, 1, &valkyrie.descriptorPool.set, 0, NULL);

		VkDeviceSize vertex_offset[1] = { 0 };
		vkCmdBindVertexBuffers(m_command_buffer.handle, 0, 1, &vertex_buffer.handle, vertex_offset);
		vkCmdBindIndexBuffer(m_command_buffer.handle, index_buffer.handle, 0, VK_INDEX_TYPE_UINT16);

		VkViewport viewport;
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = ImGui::GetIO().DisplaySize.x;
		viewport.height = ImGui::GetIO().DisplaySize.y;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_command_buffer.handle, 0, 1, &viewport);

		/*float scale[2];
		scale[0] = 2.0f / imgui_io.DisplaySize.x;
		scale[1] = 2.0f / imgui_io.DisplaySize.y;
		float translate[2];
		translate[0] = -1.0f;
		translate[1] = -1.0f;
		vkCmdPushConstants(m_command_buffer.handle, p_pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 0, sizeof(float) * 2, scale);
		vkCmdPushConstants(m_command_buffer.handle, p_pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 2, sizeof(float) * 2, translate);*/

		int vtx_offset = 0;
		int idx_offset = 0;
		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback) {
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else {
					VkRect2D scissor;
					scissor.offset.x = (int32_t)(pcmd->ClipRect.x);
					scissor.offset.y = (int32_t)(pcmd->ClipRect.y);
					scissor.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
					scissor.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y + 1);
					vkCmdSetScissor(m_command_buffer.handle, 0, 1, &scissor);
					vkCmdDrawIndexed(m_command_buffer.handle, pcmd->ElemCount, 1, idx_offset, vtx_offset, 0);
				}
				idx_offset += pcmd->ElemCount;
			}
			vtx_offset += cmd_list->VtxBuffer.Size;
		}
	}
	Vulkan::MemoryBuffer* mp_vertex_buffer;
	Vulkan::MemoryBuffer* mp_index_buffer;
	Vulkan::CommandBuffer m_command_buffer;
};

Vulkan::MemoryTexture CreateImGuiFontsTexture(Valkyrie& valkyrie, ImGuiIO& imgui_io) {
	unsigned char* pixels;
	int texture_width;
	int texture_height;
	imgui_io.Fonts->GetTexDataAsRGBA32(&pixels, &texture_width, &texture_height);

	ValkyrieImageMemoryPointer p_fonts_image = std::make_shared<ValkyrieRGBA32Memory>(texture_width, texture_height, pixels);
	Vulkan::MemoryTexture fonts_texture(p_fonts_image);
	valkyrie.initailizeTexture(fonts_texture);
	return fonts_texture;
}

int CALLBACK WinMain(HINSTANCE instance_handle, HINSTANCE, LPSTR command_line, int command_show) {
	const int width = 800;
	const int height = 600;

	Wendy::Win32Window window(width, height, instance_handle);
	std::string title("Playground");
	window.create(title);
	Valkyrie valkyrie("Valkyrie");
	valkyrie.setWindowPointer(&window);
	valkyrie.initialize();
	auto& imgui_io = ImGui::GetIO();
	imgui_io.DisplaySize.x = 800;
	imgui_io.DisplaySize.y = 600;

	valkyrie.vertexInput.setBindingDescription(0, sizeof(Vertex));
	valkyrie.vertexInput.setAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
	valkyrie.vertexInput.setAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, 3 * sizeof(float));

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

	Vulkan::MemoryBuffer vertex_buffer;
	Vulkan::MemoryBuffer index_buffer;
	Vulkan::MemoryBuffer uniform_buffer;
	ValkyrieImageFilePointer png_ptr = std::make_shared<ValkyriePNG>();
	Vulkan::ImageTexture texture(png_ptr);

	valkyrie.allocateMemoryBuffer(vertex_buffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexs.size() * sizeof(Vertex));
	valkyrie.allocateMemoryBuffer(index_buffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices.size() * sizeof(uint32_t));
	valkyrie.allocateMemoryBuffer(uniform_buffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(mvp));
	valkyrie.writeMemoryBuffer(vertex_buffer, vertexs.data());
	valkyrie.writeMemoryBuffer(index_buffer, indices.data());
	valkyrie.writeMemoryBuffer(uniform_buffer, &mvp);
	texture.load("wang.png");
	valkyrie.initailizeTexture(texture);

	Vulkan::MemoryTexture memory_texture = CreateImGuiFontsTexture(valkyrie, imgui_io);
	imgui_io.Fonts->TexID = memory_texture.image;

	valkyrie.descriptorPool.setLayout.setBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);
	valkyrie.descriptorPool.setLayout.setBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	valkyrie.initializeDescriptorSetLayout();

	valkyrie.initializePipelineLayout();
	valkyrie.pipelines["NORMAL"] = std::make_shared<Vulkan::Pipeline>();
	valkyrie.pipelines["IMGUI"] = std::make_shared<Vulkan::Pipeline>();
	auto p_normal_pipeline = valkyrie.pipelines["NORMAL"];
	auto p_imgui_pipeline = valkyrie.pipelines["IMGUI"];

	std::string normal_vertex_code = Vulkan::Shader::LoadSPVBinaryCode("vert.spv");
	std::string normal_fragment_code = Vulkan::Shader::LoadSPVBinaryCode("frag.spv");
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

	valkyrie.initializePipelines();
	valkyrie.descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
	valkyrie.descriptorPool.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	valkyrie.initializeDescriptorPool();
	valkyrie.initializeDescriptorSets();

	std::vector<VkWriteDescriptorSet> writes(2);
	VkWriteDescriptorSet& write_uniform = writes[0];
	VkWriteDescriptorSet& samplers = writes[1];

	write_uniform = {};
	write_uniform.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write_uniform.dstSet = valkyrie.descriptorPool.set;
	write_uniform.descriptorCount = 1;
	write_uniform.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write_uniform.pBufferInfo = uniform_buffer.getInformationPointer();
	write_uniform.dstBinding = 0;

	samplers = {};
	samplers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	samplers.dstSet = valkyrie.descriptorPool.set;
	samplers.descriptorCount = 1;
	samplers.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplers.pImageInfo = texture.getInformationPointer();
	samplers.dstBinding = 1;

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

		vkCmdBindDescriptorSets(command.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, p_normal_pipeline->layout, 0, 1, &valkyrie.descriptorPool.set, 0, nullptr);
		vkCmdBindPipeline(command.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, p_normal_pipeline->handle);

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
	ValkyrieRenderPFN imgui_render_pfn = std::make_shared<ImGuiRenderFunction>();
	valkyrie.registerRenderFunction("imgui", imgui_render_pfn);
	std::vector<void*> parameter({ &valkyrie, &p_normal_pipeline });
	while (valkyrie.execute()) {
		rotation.x += 0.01f;
		rotation.y += 0.01f;
		mvp.model = glm::mat4();
		mvp.model = glm::rotate(mvp.model, rotation.x * 3.14f / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		mvp.model = glm::rotate(mvp.model, rotation.y * 3.14f / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		mvp.model = glm::rotate(mvp.model, rotation.z * 3.14f / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		valkyrie.writeMemoryBuffer(uniform_buffer, &mvp);
		valkyrie.executeRenderFunction("imgui", parameter);
	}
	return 0;
}