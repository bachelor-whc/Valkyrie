#ifndef _VULKAN_TEXTURE_H
#define _VULKAN_TEXTURE_H
#include "utility.h"
#include "valkyrie/texture.h"

namespace Vulkan {
	struct Device;
	struct PhysicalDevice;
	struct Queue;
	struct CommandBuffer;

	struct Texture {
		Texture() = delete;
		Texture(const ValkyriePNGPointer& texture_ptr);
		~Texture();

		VkResult initializeImage(const Device& device);
		VkResult initializeSampler(const Device& device);
		VkResult initializeView(const Device& device);
		VkResult allocate(const Device& device, PhysicalDevice& physical_device);
		VkResult write(const Device& device);
		VkDescriptorImageInfo* getInformationPointer();
		inline bool load(const std::string& file_path) { return mp_png->load(file_path); };

		VkDeviceMemory memory = NULL;
		VkImage image = NULL;
		VkImageView view = NULL;
		VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		VkSampler sampler = NULL;
	private:
		ValkyriePNGPointer mp_png;
		uint64_t m_size;
		VkDescriptorImageInfo* mp_information;
	};
}

#endif