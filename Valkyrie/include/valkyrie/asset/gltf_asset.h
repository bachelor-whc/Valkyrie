#ifndef _VALKYRIE_GLTF_ASSET_H 
#define _VALKYRIE_GLTF_ASSET_H 
#include <json.hpp>
#include "common.h"
#include "valkyrie/asset/asset.h"
#include "valkyrie/vulkan/memory_buffer.h"
#include "valkyrie/memory_chunk.h"
#include "valkyrie/graphics_api_support/attribute.h"

namespace Valkyrie {
	enum GLTF_COMPONENT_TYPE {
		BYTE = 5120,
		UNSIGNED_BYTE = 5121,
		SHORT = 5122,
		UNSIGNED_SHORT = 5123,
		FLOAT = 5126
	};
	enum GLTF_TYPE {
		SCALAR, VEC2, VEC3, VEC4, MAT2, MAT3, MAT4
	};

	class glTFBufferView : 
		public Asset, 
		public MemoryAccess {
	public:
		glTFBufferView() = delete;
		glTFBufferView(const MemoryChunkPtr& buffer_ptr, const uint32_t length, const uint32_t offset);
		virtual ~glTFBufferView();

		virtual void* getData() { return (unsigned char*)m_buffer_ptr->getData() + m_offset; }
		virtual uint32_t getSize() const { return m_length; }

		virtual bool allocated() const { return m_buffer_ptr->allocated(); }
		virtual bool ready() const { return m_buffer_ptr->ready(); }
	private:
		MemoryChunkPtr m_buffer_ptr;
		uint32_t m_length;
		uint32_t m_offset;
	};

	typedef std::shared_ptr<glTFBufferView> glTFBufferViewPtr;

	class glTFAccessor : 
		public GrpahicsAPIAttributeSupport {
	public:
		glTFAccessor() = delete;
		glTFAccessor(const GAPIAttributeSupportPtr& gaas_ptr, GLTF_TYPE type, GLTF_COMPONENT_TYPE component_type);
		virtual ~glTFAccessor();

		virtual void* getData() { return m_attribute_ptr->getData(); }
		virtual uint32_t getSize() const { return m_attribute_ptr->getSize(); }
		virtual uint32_t getTypeSize() const { return m_attribute_ptr->getTypeSize(); }
		virtual uint32_t getOffset() const { return m_attribute_ptr->getOffset(); }
		virtual uint32_t getStride() const { return m_attribute_ptr->getStride(); }
		virtual uint32_t getCount() const { return m_attribute_ptr->getCount(); }
		virtual VkFormat getFormat() const { return m_attribute_ptr->getFormat(); }
		virtual VkIndexType getIndexFormat() const { return m_attribute_ptr->getIndexFormat(); }
		inline GLTF_TYPE getglTFType() const { return m_type; }
		inline GLTF_COMPONENT_TYPE getglTFComponentType() const { return m_component_type; }
		virtual void* getInstance(uint32_t index) { return m_attribute_ptr->getInstance(index); }

		virtual bool allocated() const { return m_attribute_ptr->allocated(); }
		virtual bool ready() const { return m_attribute_ptr->ready(); }
	private:
		GLTF_TYPE m_type;
		GLTF_COMPONENT_TYPE m_component_type;
		GAPIAttributeSupportPtr m_attribute_ptr;
	};

	typedef std::shared_ptr<glTFAccessor> glTFAccessorPtr;

	class glTFAsset : public Asset {
	public:
		glTFAsset() {};
		glTFAsset(const JSON& src);
		virtual ~glTFAsset() {};

		virtual void* getData() { return &m_json; };
		virtual uint32_t getSize() const { return 0; };
		void setJSON(const JSON& src);

		std::set<std::string> bufferSet;
		std::set<std::string> bufferViewSet;
		std::set<std::string> accessorSet;
		std::set<std::string> uriSet;

		void setBuffer(const std::string& name, const MemoryChunkPtr& buffer);
		void setBufferView(const std::string& name, const glTFBufferViewPtr& buffer_view);
		void setAccessor(const std::string& name, const glTFAccessorPtr& accessor);

		glTFAccessorPtr getAccessor(const std::string& name) throw(...);

	private:
		JSON m_json;
		std::map<std::string, MemoryChunkPtr> m_buffer_ptrs;
		std::map<std::string, glTFBufferViewPtr> m_buffer_view_ptrs;
		std::map<std::string, glTFAccessorPtr> m_accessor_ptrs;
	};

	typedef std::shared_ptr<glTFAsset> glTFAssetPtr;
}

#endif