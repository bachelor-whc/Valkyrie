#ifndef _VALKYRIE_GLTF_ASSET_H 
#define _VALKYRIE_GLTF_ASSET_H 
#include <json.hpp>
#include "common.h"
#include "valkyrie/asset/asset.h"
#include "valkyrie/asset/mesh_asset.h"
#include "valkyrie/vulkan/memory_buffer.h"
#include "valkyrie/graphics_api_support/attribute.h"

namespace ValkyrieFactory {
	class MeshFactory;
}

namespace Valkyrie {
	class MemoryChunk;
	using MemoryChunkPtr = std::shared_ptr<MemoryChunk>;

	class LavyAsset : 
		public Asset,
		public Valkyrie::MeshSupport {
	public:
		LavyAsset() = delete;
		LavyAsset(const JSON& src);
		virtual ~LavyAsset() {};

		virtual uint32_t getDrawVertexCount();
		virtual uint32_t getVerticeBufferOffset();
		virtual uint32_t getIndiceBufferOffset();
		virtual uint32_t getVerticeBufferLength();
		virtual uint32_t getIndiceBufferLength();

	private:
		friend class LavyLoader;
		friend class ValkyrieFactory::MeshFactory;

		JSON m_json;
		MemoryChunkPtr m_buffer_ptr;
		uint32_t m_vertices_byte_length = 0;
		uint32_t m_vertices_byte_offset = 0;
		uint32_t m_indices_byte_length = 0;
		uint32_t m_indices_byte_offset = 0;
		uint32_t m_indices_count = 0;
	};

	using LavyAssetPtr = std::shared_ptr<LavyAsset>;
}

#endif