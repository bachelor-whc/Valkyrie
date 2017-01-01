#ifndef _VULKAN_MEMORY_TEXTURE_H
#define _VULKAN_MEMORY_TEXTURE_H
#include "common.h"
#include "valkyrie/vulkan/texture.h"

namespace Vulkan {
	class MemoryTexture : public Texture {
	public:
		MemoryTexture() = delete;
		MemoryTexture(const Valkyrie::ImageMemoryPointer& texture_ptr);
		virtual VkResult initializeImage() override;
		virtual VkResult allocate()  override;
		virtual VkResult write()  override;
		~MemoryTexture();

	private:
		Valkyrie::ImageMemoryPointer mp_memory = nullptr;
		uint32_t m_size;
	};
}

#endif