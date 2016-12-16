#ifndef _VULKAN_TEXTURE_H
#define _VULKAN_TEXTURE_H
#include "utility.h"
#include "valkyrie/image.h"

namespace Vulkan {
	struct Device;
	struct PhysicalDevice;
	struct Queue;
	struct CommandBuffer;

	class Texture {
	public:
		Texture();
		~Texture();

		virtual VkResult initializeImage(const Device& device) = 0;
		VkResult initializeSampler(const Device& device);
		VkResult initializeView(const Device& device);
		virtual VkResult allocate(const Device& device, PhysicalDevice& physical_device) = 0;
		virtual VkResult write(const Device& device) = 0;
		VkDescriptorImageInfo* getInformationPointer();

		VkDeviceMemory memory = NULL;
		VkImage image = NULL;
		VkImageView view = NULL;
		VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		VkSampler sampler = NULL;
	private:
		VkDescriptorImageInfo* mp_information = nullptr;
	};
}

#endif