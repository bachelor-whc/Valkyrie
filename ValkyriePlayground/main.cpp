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

using namespace Valkyrie;

struct ModelViewProjection {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

ImageMemoryPointer loadPNG(const std::string file_path) {
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

int CALLBACK WinMain(HINSTANCE instance_handle, HINSTANCE, LPSTR command_line, int command_show) {
	ValkyrieEngine::initializeValkyrieEngine();
	auto& valkyrie = *ValkyrieEngine::getGlobalValkyriePtr();

	auto& asset_manager = *AssetManager::getGlobalAssetMangerPtr();
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
	auto image_ptr = loadPNG("assets/gltf/test.png");
	Vulkan::Texture texture(image_ptr);
	VulkanManager::initializeTexture(texture);

	normal_uniform_buffer.allocate({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT }, sizeof(mvp));
	normal_uniform_buffer.write(&mvp, 0, sizeof(mvp));

	Graphics::Pipeline pipeline;
	pipeline.descriptorPoolPtr = MAKE_SHARED(Vulkan::DescriptorPool)();
	pipeline.descriptorPoolPtr->initializePool(1);
	pipeline.descriptorPoolPtr->setLayouts[0].setBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);
	pipeline.descriptorPoolPtr->setLayouts[0].setBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	pipeline.descriptorPoolPtr->initializeSets();

	while (valkyrie.execute()) {
		
	}
	ValkyrieEngine::closeValkyrieEngine();
	return 0;
}
