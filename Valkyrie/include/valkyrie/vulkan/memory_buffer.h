#ifndef _VULKAN_MEMORY_BUFFER_H
#define _VULKAN_MEMORY_BUFFER_H
#include "utility.h"

namespace Vulkan {
	struct PhysicalDevice;
	struct Device;

	struct MemoryBuffer {
		MemoryBuffer();
		~MemoryBuffer();

		VkResult allocate(PhysicalDevice& physical_device, const VkBufferUsageFlags usage, uint32_t size, VkBufferCreateInfo buffer_create);
		VkResult write(const void *data, uint32_t offset = 0);
		void* startWriting(uint32_t offset = 0);
		VkResult endWriting();
		VkDescriptorBufferInfo* getInformationPointer();
		inline uint32_t getSize() { return m_size; }
		inline uint32_t getOffset() { return m_size; }

		VkBuffer handle = NULL;
		VkDeviceMemory memory = NULL;
	private:
		uint32_t m_size;
		uint32_t m_offset;
		VkDescriptorBufferInfo* mp_information;
		bool m_writing_state = false;
	};
}

#endif