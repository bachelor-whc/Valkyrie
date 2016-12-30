#ifndef _VULKAN_IMAGE_TEXTURE_H
#define _VULKAN_IMAGE_TEXTURE_H
#include "utility.h"
#include "valkyrie/vulkan/texture.h"

namespace Vulkan {
	class ImageTexture : public Texture {
	public:
		ImageTexture() = delete;
		ImageTexture(const Valkyrie::ImageFilePointer& texture_ptr);
		virtual VkResult initializeImage() override;
		virtual VkResult allocate()  override;
		virtual VkResult write()  override;
		virtual ~ImageTexture();

		virtual bool load(const std::string& file_path) { return mp_image->load(file_path); };
	private:
		Valkyrie::ImageFilePointer mp_image = nullptr;
		uint32_t m_size;
	};
}

#endif