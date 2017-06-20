#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <valkyrie/valkyrie.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <imgui.h>
#include <stb_image.h>
using namespace Valkyrie;

using ModelViewProjections = std::vector<glm::mat4>;
using ObjectList = std::vector<unsigned int>;
using VisibleList = std::vector<int>;
std::vector<ObjectList> thread_IDs;
std::vector<ModelViewProjections> thread_MVPs;
std::vector<VisibleList> thread_visiblelists;

ImageMemoryPointer LoadPNG(const std::string file_path) {
	FILE* p_png_file = fopen(file_path.c_str(), "rb");
	int png_w;
	int png_h;
	int png_c;
	void* wang_png_ptr = stbi_load_from_file(p_png_file, &png_w, &png_h, &png_c, STBI_rgb_alpha);
	fclose(p_png_file);
	ImageMemoryPointer p_image = std::make_shared<RGBA32Memory>(png_w, png_h);
	memcpy(p_image->getData(), wang_png_ptr, p_image->getSize());
	p_image->setFlags(MemoryAccess::READY);
	return p_image;
}

void CreateThreadRenderData(std::vector<Vulkan::ThreadCommandPoolPtr>& thread_ptrs, int num_of_threads, int num_of_objects) {
	thread_ptrs.resize(num_of_threads);
	thread_IDs.resize(num_of_threads);
	thread_MVPs.resize(num_of_threads);
	thread_visiblelists.resize(num_of_threads);
	auto& queue = Valkyrie::VulkanManager::getGraphicsQueue();
	auto& factory = ValkyrieFactory::ObjectFactory::instance();
	auto& manager = Valkyrie::ObjectManager::instance();
	std::uniform_real_distribution<float> uniform_distribution(-750.0f, 750.0f);
	auto& random_generator = manager.getRandomGenerator();
	int num_of_objects_per_thread = num_of_objects / num_of_threads;
	for (int i = 0; i < num_of_threads; ++i) {
		thread_ptrs[i] = MAKE_SHARED(Vulkan::ThreadCommandPool)(queue);
		thread_ptrs[i]->initializeSecondaryCommandBuffers(num_of_objects_per_thread);
		thread_MVPs[i].resize(num_of_objects_per_thread);
		thread_visiblelists[i].resize(num_of_objects_per_thread);
		for (int j = 0; j < num_of_objects_per_thread; ++j) {
			auto& object_ptr = factory.createObject();
			thread_IDs[i].push_back(object_ptr->getID());
			object_ptr->transform.setScale(0.1f, 0.1f, 0.1f);
			object_ptr->transform.setTranslate(
				uniform_distribution(random_generator),
				uniform_distribution(random_generator),
				uniform_distribution(random_generator)
			);
		}
	}
}

void ReleaseThreadRenderData(std::vector<Vulkan::ThreadCommandPoolPtr>& thread_ptrs) {
	for (auto& tp : thread_ptrs) {
		Vulkan::DestroyCommandPool(*tp);
	}
}

int CALLBACK WinMain(HINSTANCE instance_handle, HINSTANCE, LPSTR command_line, int command_show) {
	ValkyrieEngine::initializeValkyrieEngine();
	auto& valkyrie = ValkyrieEngine::instance();
	auto& factory = ValkyrieFactory::ObjectFactory::instance();

	std::vector<Vulkan::ThreadCommandPoolPtr> thread_ptrs;
	int num_of_threads = Valkyrie::TaskManager::instance().getNumberOfThreads();
	int num_of_objects = 1024;
	int num_of_objects_per_thread = num_of_objects / num_of_threads;
	CreateThreadRenderData(thread_ptrs, num_of_threads, num_of_objects);

	auto& asset_manager = AssetManager::instance();
	asset_manager.load("duck.lavy");
	asset_manager.load("duck.lavy");
	auto& mesh_ptr = asset_manager.getMesh("LOD3sp");
	ValkyrieComponent::MeshRenderer mesh_renderer(mesh_ptr);
	Scene::Object duck;
	auto& camera_ptr = factory.createCamera(60, 1024.0f/768.0f, 0.1f, 1000.0f);

	auto image_ptr = LoadPNG("assets/gltf/test.png");
	Vulkan::Texture texture(image_ptr);
	VulkanManager::initializeTexture(texture);

	Graphics::Pipeline pipeline;
	pipeline.descriptorPoolPtr = MAKE_SHARED(Vulkan::DescriptorPool)();
	pipeline.descriptorPoolPtr->addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
	pipeline.descriptorPoolPtr->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	pipeline.descriptorPoolPtr->initializePool(1);
	pipeline.descriptorPoolPtr->setLayouts[0].setBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);
	pipeline.descriptorPoolPtr->setLayouts[0].setBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	pipeline.descriptorPoolPtr->initializeSets();
	pipeline.vertexInput.setBindingDescription(0, 32);
	pipeline.vertexInput.setAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
	pipeline.vertexInput.setAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, 12);
	pipeline.vertexInput.setAttributeDescription(0, 2, VK_FORMAT_R32G32_SFLOAT, 24);
	std::string vertex_code = Vulkan::Shader::LoadSPVBinaryCode("shader.vert.spv");
	std::string fragment_code = Vulkan::Shader::LoadSPVBinaryCode("shader.frag.spv");
	auto vertex_shader = MAKE_SHARED(Vulkan::Shader)(vertex_code, VK_SHADER_STAGE_VERTEX_BIT);
	auto fragment_shader = MAKE_SHARED(Vulkan::Shader)(fragment_code, VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline.shaderPtrs[Graphics::Pipeline::ShaderStage::VERTEX] = vertex_shader;
	pipeline.shaderPtrs[Graphics::Pipeline::ShaderStage::FRAGMENT] = fragment_shader;
	pipeline.module.pushConstantRanges.resize(1);
	pipeline.module.pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pipeline.module.pushConstantRanges[0].offset = 0;
	pipeline.module.pushConstantRanges[0].size = sizeof(glm::mat4);

	auto& renderer = valkyrie.getRenderer();
	pipeline.initialize(renderer);

	pipeline.descriptorPoolPtr->updateDescriptorSet(texture, 0, 1);

	auto ry = 0.0f;

	VkRenderPassBeginInfo render_pass_begin = renderer.getRenderPassBegin();

	auto& task_manager = Valkyrie::TaskManager::instance();
	auto& object_manager = Valkyrie::ObjectManager::instance();

	while (valkyrie.execute()) {
		renderer.prepareFrame();

		int current = renderer.getCurrentBuffer();
		auto& command = renderer.renderCommands[current];
		command.begin();
		render_pass_begin.framebuffer = renderer.getFramebuffer(current);
		vkCmdBeginRenderPass(command.handle, &render_pass_begin, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		VkCommandBufferInheritanceInfo inheritance = {};
		inheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritance.renderPass = renderer.getRenderPassHandle();
		inheritance.framebuffer = renderer.getFramebuffer(current);
		ry += 1.0f;
		camera_ptr->transform.getTranslteRef().z = 500.0f + sinf(ry / 100.f) * 500.0f;
		camera_ptr->update();
		auto& camera_properties = camera_ptr->getProperties();
		
		for (int i = 0; i < num_of_threads; ++i) {
			auto& thread = *thread_ptrs[i];
			auto& objects = thread_IDs[i];
			auto& MVPs = thread_MVPs[i];
			auto& visiblelist = thread_visiblelists[i];
			task_manager.group.run([&]() {
				for (int c = 0; c < num_of_objects_per_thread; ++c) {
					int ID = objects[c];
					auto& object = *object_manager.getObjectPtr(objects[c]);
					
					object.transform.getRotationRef().x = glm::radians<float>(ry);
					object.transform.getRotationRef().z = glm::radians<float>(ry);

					MVPs[c] = camera_properties.getPerspective() * camera_properties.getView() * object.transform.getWorldMatrix();
					
					visiblelist[c] = camera_properties.frustum.checkPosition(object.transform.getTranslteValue()) ? 1 : 0;
					if (visiblelist[c] == 0)
						continue;

					Vulkan::InheritanceCommandBuffer icb(thread.commands[c], inheritance);
					icb.begin();
					renderer.commandSetViewport(icb);
					renderer.commandSetScissor(icb);
					pipeline.commandBind(icb);

					vkCmdPushConstants(
						icb.handle,
						pipeline.module.layout,
						VK_SHADER_STAGE_VERTEX_BIT,
						0,
						sizeof(glm::mat4),
						&MVPs[c]
					);
					mesh_renderer.recordDrawCommand(icb);
					icb.end();
				}
			});
		}
		task_manager.group.wait();

		std::vector<VkCommandBuffer> recorded_commands;
		for(int t = 0; t < num_of_threads; ++t) {
			for (int tvli = 0; tvli < num_of_objects_per_thread; ++tvli) {
				if(thread_visiblelists[t][tvli] > 0)
					recorded_commands.push_back(thread_ptrs[t]->commands[tvli]);
			}
		}

		vkCmdExecuteCommands(
			command.handle,
			recorded_commands.size(),
			recorded_commands.data()
		);

		vkCmdEndRenderPass(command.handle);
		command.end();

		renderer.render();
	}

	ReleaseThreadRenderData(thread_ptrs);
	ValkyrieEngine::closeValkyrieEngine();
	return 0;
}
