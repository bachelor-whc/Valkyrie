#ifndef _VALKYRIE_GRAPHICS_API_ATTRIBUTE_H 
#define _VALKYRIE_GRAPHICS_API_ATTRIBUTE_H 
#include "utility.h"

namespace Valkyrie {
	class GrpahicsAPIAttributeSupport {
	public:
		GrpahicsAPIAttributeSupport() {}
		virtual ~GrpahicsAPIAttributeSupport() {};
		virtual uint32_t getTypeSize() const = 0;
		virtual uint32_t getOffset() const = 0;
		virtual uint32_t getStride() const = 0;
		virtual uint32_t getCount() const = 0;
		virtual VkFormat getFormat() const = 0;
		virtual VkIndexType getIndexFormat() const = 0;
		virtual void* getInstance(uint32_t index) = 0;
	};

	typedef std::shared_ptr<GrpahicsAPIAttributeSupport> GAPIAttributeSupportPtr;

	template<typename T>
	class GrpahicsAPIAttribute : public GrpahicsAPIAttributeSupport {
	public:
		GrpahicsAPIAttribute() = delete;
		GrpahicsAPIAttribute(const MemoryAccessPtr& ptr, const uint32_t offset = 0, const uint32_t stride = 0, const uint32_t count = 1);
		virtual ~GrpahicsAPIAttribute() {};
		virtual uint32_t getTypeSize() const { return m_type_size; };
		virtual uint32_t getOffset() const { return m_offset; }
		virtual uint32_t getStride() const { return m_stride; }
		virtual uint32_t getCount() const { return m_count; }
		virtual VkFormat getFormat() const { return m_format; }
		virtual VkIndexType getIndexFormat() const { return m_index_format; }
		virtual void* getInstance(uint32_t index);

	private:
		void initializeFormats();

		T* mp_implement = nullptr;
		uint32_t m_type_size;
		uint32_t m_offset;
		uint32_t m_stride;
		uint32_t m_count;
		VkFormat m_format = VK_FORMAT_MAX_ENUM;
		VkIndexType m_index_format = VK_INDEX_TYPE_MAX_ENUM;
	};
}
#endif