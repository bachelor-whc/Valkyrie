#ifndef _VULKAN_TEXTURE_H
#define _VULKAN_TEXTURE_H
#include "common.h"
#include "valkyrie/image.h"
#include "valkyrie/vulkan/image.h"

namespace Vulkan {
	struct Device;
	struct PhysicalDevice;
	struct Queue;
	struct CommandBuffer;

	class Texture : public Image {
	public:
		Texture() = delete;
		Texture(const Valkyrie::ImageMemoryPointer& image_ptr);
		virtual ~Texture();

		virtual VkImageCreateInfo getImageCreate() const;
		virtual VkImageViewCreateInfo getImageViewCreate() const;
		VkResult initializeSampler();
		VkResult write();
		VkDescriptorImageInfo* getInformationPointer();

		VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		VkSampler sampler = VK_NULL_HANDLE;
	private:
		VkDescriptorImageInfo* mp_information = nullptr;
		Valkyrie::ImageMemoryPointer m_image_ptr;
		uint32_t m_size = 0;
	};
}

#endif