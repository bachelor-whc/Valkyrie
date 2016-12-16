#ifndef _VULKAN_MEMORY_TEXTURE_H
#define _VULKAN_MEMORY_TEXTURE_H
#include "utility.h"
#include "valkyrie/vulkan/texture.h"

namespace Vulkan {
	class MemoryTexture : public Texture {
	public:
		MemoryTexture() = delete;
		MemoryTexture(const ValkyrieImageMemoryPointer& texture_ptr);
		virtual VkResult initializeImage(const Device& device) override;
		virtual VkResult allocate(const Device& device, PhysicalDevice& physical_device)  override;
		virtual VkResult write(const Device& device)  override;
		~MemoryTexture();

	private:
		ValkyrieImageMemoryPointer mp_memory = nullptr;
		uint32_t m_size;
	};
}

#endif