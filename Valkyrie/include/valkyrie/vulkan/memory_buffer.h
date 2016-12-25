#ifndef _VULKAN_MEMORY_BUFFER_H
#define _VULKAN_MEMORY_BUFFER_H
#include "utility.h"
#include "valkyrie/default_create_info.h"

namespace Vulkan {
	struct PhysicalDevice;
	struct Device;

	struct MemoryBuffer {
		MemoryBuffer();
		~MemoryBuffer();

		VkResult allocate(const std::vector<VkBufferUsageFlags>& usages, const std::vector<uint32_t>& sizes, VkBufferCreateInfo buffer_create = VK_DEFAULT_BUFFER_CREATE_INFO);
		VkResult write(const void *data, int index);
		void* startWriting(int index);
		void endWriting();
		VkDescriptorBufferInfo* getInformationPointer(int index);
		inline uint32_t getSize(int index) { return m_sizes[index]; }
		inline uint32_t getOffset(int index) { return m_offsets[index]; }

		VkBuffer handle = NULL;
		VkDeviceMemory memory = NULL;
	private:
		std::vector<uint32_t> m_sizes;
		std::vector<uint32_t> m_offsets;
		std::vector<VkDescriptorBufferInfo*> m_information_pointers;
		bool m_writing_state = false;
	};

	void DestroyMemoryBuffer(MemoryBuffer& buffer);
}

#endif