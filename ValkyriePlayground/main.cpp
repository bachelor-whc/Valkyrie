#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <valkyrie.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <imgui.h>
#include <stb_image.h>
#include <valkyrie/graphics/pipeline.h>
#include <valkyrie/vulkan/thread.h>
using namespace Valkyrie;

struct ModelViewProjection {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

using ObjectList = std::vector<unsigned int>;
std::vector<ObjectList> thread_data;

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
	thread_data.resize(num_of_threads);
	auto& queue = Valkyrie::VulkanManager::getGraphicsQueue();
	auto& factory = ValkyrieFactory::ObjectFactory::instance();
	auto& manager = Valkyrie::ObjectManager::instance();
	std::uniform_real_distribution<float> uniform_distribution(-1.0f, 1.0f);
	auto& random_generator = manager.getRandomGenerator();
	int num_of_objects_per_thread = num_of_objects / num_of_threads;
	for (int i = 0; i < num_of_threads; ++i) {
		thread_ptrs[i] = MAKE_SHARED(Vulkan::ThreadCommandPool)(queue);
		thread_ptrs[i]->initializeSecondaryCommandBuffers(num_of_objects_per_thread);
		for (int j = 0; j < num_of_objects_per_thread; ++j) {
			auto& object_ptr = factory.createObject();
			thread_data[i].push_back(object_ptr->getID());
			object_ptr->transform.setScale(0.01f, 0.01f, 0.01f);
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

	std::vector<Vulkan::ThreadCommandPoolPtr> thread_ptrs;
	CreateThreadRenderData(thread_ptrs, 4, 1000);

	auto& asset_manager = AssetManager::instance();
	asset_manager.load("duck.lavy");
	auto& mesh_ptr = std::static_pointer_cast<Mesh>(asset_manager.getAsset("duck.lavy"));
	ValkyrieComponent::MeshRenderer mesh_renderer(mesh_ptr);
	ValkyrieComponent::CameraPtr camera_ptr = MAKE_SHARED(ValkyrieComponent::Camera)();
	Scene::Object duck;
	Scene::ObjectPtr camera_obj_ptr = MAKE_SHARED(Scene::Object)();
	Valkyrie::ComponentAttacher attacher;
	attacher.attachComponent(camera_obj_ptr, camera_ptr);
	camera_ptr->setRatio(1024, 768);
	camera_ptr->update();

	ModelViewProjection mvp;
	mvp.view = camera_ptr->getView();
	mvp.projection = camera_ptr->getPerspective();

	Vulkan::MemoryBuffer normal_uniform_buffer;
	auto image_ptr = LoadPNG("assets/gltf/test.png");
	Vulkan::Texture texture(image_ptr);
	VulkanManager::initializeTexture(texture);

	normal_uniform_buffer.allocate({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT }, sizeof(mvp));
	normal_uniform_buffer.write(&mvp, 0, sizeof(mvp));

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
	
	auto render_context_ptr = valkyrie.getRenderContextPtr();
	pipeline.initialize(render_context_ptr);

	pipeline.descriptorPoolPtr->updateDescriptorSet(normal_uniform_buffer, 0, 0);
	pipeline.descriptorPoolPtr->updateDescriptorSet(texture, 0, 1);

	VkRenderPassBeginInfo render_pass_begin = render_context_ptr->getRenderPassBegin();
	int render_command_size = render_context_ptr->renderCommands.size();
	
	VkResult result;
	for (int i = 0; i < render_command_size; ++i) {
		auto& command = render_context_ptr->renderCommands[i];
		command.begin();
		render_pass_begin.framebuffer = render_context_ptr->getFramebuffer(i);
		vkCmdBeginRenderPass(command.handle, &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

		render_context_ptr->commandSetViewport(command);
		render_context_ptr->commandSetScissor(command);

		pipeline.commandBind(command);
		mesh_renderer.recordDrawCommand(command);

		vkCmdEndRenderPass(command.handle);
		result = command.end();
		assert(result == VK_SUCCESS);
	}

	auto& duck_transform = duck.transform;
	duck_transform.setRotation(0.0f, 0.0f, -90.0f);
	auto ty = 0.0f;
	auto ry = 0.0f;
	auto s = 0.01f;
	duck_transform.setScale(s, s, s);

	while (valkyrie.execute()) {
		camera_obj_ptr->update();
		camera_obj_ptr->transform.getTranslteRef().z = 25.0f * sin(ty) + 30.0f;
		camera_obj_ptr->transform.getRotationRef().z = glm::radians<float>(ry);
		duck_transform.getTranslteRef().y = sin(ty);
		duck_transform.getRotationRef().x = glm::radians<float>(ry);
		ry += 1.0f;
		ty += 0.01f;
		mvp.view = camera_ptr->getView();
		mvp.model = duck_transform.getWorldMatrix();
		normal_uniform_buffer.write(&mvp, 0, sizeof(mvp));
	}
	render_context_ptr.reset();

	ReleaseThreadRenderData(thread_ptrs);
	ValkyrieEngine::closeValkyrieEngine();
	return 0;
}
