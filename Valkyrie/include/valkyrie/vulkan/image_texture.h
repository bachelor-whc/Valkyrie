#ifndef _VULKAN_IMAGE_TEXTURE_H
#define _VULKAN_IMAGE_TEXTURE_H
#include "utility.h"
#include "valkyrie/vulkan/texture.h"

namespace Vulkan {
	class ImageTexture : public Texture {
	public:
		ImageTexture() = delete;
		ImageTexture(const ValkyrieImageFilePointer& texture_ptr);
		virtual VkResult initializeImage(const Device& device) override;
		virtual VkResult allocate(const Device& device, PhysicalDevice& physical_device)  override;
		virtual VkResult write(const Device& device)  override;
		virtual ~ImageTexture();

		virtual bool load(const std::string& file_path) { return mp_image->load(file_path); };
	private:
		ValkyrieImageFilePointer mp_image = nullptr;
		uint32_t m_size;
	};
}

#endif