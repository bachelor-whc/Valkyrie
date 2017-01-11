#ifndef _VULKAN_MEMORY_BUFFER_H
#define _VULKAN_MEMORY_BUFFER_H
#include "common.h"
#include "valkyrie/default_create_info.h"

namespace Vulkan {
	struct PhysicalDevice;
	struct Device;

	struct MemoryBuffer {
		MemoryBuffer();
		~MemoryBuffer();

		VkResult allocate(const std::vector<VkBufferUsageFlags>& usages, const uint32_t size, VkBufferCreateInfo buffer_create = VK_DEFAULT_BUFFER_CREATE_INFO);
		VkResult write(const void *data, const uint32_t offset, const uint32_t size);
		void* startWriting(const uint32_t offset, const uint32_t size);
		void endWriting();
		VkDescriptorBufferInfo* getInformationPointer();
		VkDescriptorBufferInfo* getInformationPointer(const uint32_t offset, const uint32_t size);
		inline uint32_t getSize() { return m_size; }

		VkBuffer handle = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	private:
		uint32_t m_size = 0;
		VkDescriptorBufferInfo* mp_information = nullptr;
		bool m_writing_state = false;
	};

	void DestroyMemoryBuffer(MemoryBuffer& buffer);
	typedef std::shared_ptr<MemoryBuffer> MemoryBufferPtr;
}

#endif