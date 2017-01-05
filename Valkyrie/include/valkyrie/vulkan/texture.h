#ifndef _VULKAN_TEXTURE_H
#define _VULKAN_TEXTURE_H
#include "common.h"
#include "valkyrie/image.h"

namespace Vulkan {
	struct Device;
	struct PhysicalDevice;
	struct Queue;
	struct CommandBuffer;

	class Texture {
	public:
		Texture() = delete;
		Texture(const Valkyrie::ImageMemoryPointer& image_ptr);
		virtual ~Texture();

		VkResult initializeImage();
		VkResult initializeSampler();
		VkResult initializeView();
		VkResult allocate();
		VkResult write();
		VkDescriptorImageInfo* getInformationPointer();

		VkDeviceMemory memory = NULL;
		VkImage image = NULL;
		VkImageView view = NULL;
		VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		VkSampler sampler = NULL;
	private:
		VkDescriptorImageInfo* mp_information = nullptr;
		Valkyrie::ImageMemoryPointer m_image_ptr;
		uint32_t m_size = 0;
	};
}

#endif