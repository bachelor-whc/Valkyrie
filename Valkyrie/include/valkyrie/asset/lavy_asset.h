#ifndef _VALKYRIE_GLTF_ASSET_H 
#define _VALKYRIE_GLTF_ASSET_H 
#include <json.hpp>
#include "common.h"
#include "valkyrie/asset/asset.h"
#include "valkyrie/vulkan/memory_buffer.h"
#include "valkyrie/memory_chunk.h"
#include "valkyrie/graphics_api_support/attribute.h"

namespace Valkyrie {
	class LavyAsset : public Asset {
	public:
		LavyAsset() = delete;
		LavyAsset(const JSON& src);
		virtual ~LavyAsset() {};

	private:
		friend class LavyLoader;

		JSON m_json;
		MemoryChunkPtr m_buffer_ptr;
		uint32_t m_vertices_byte_length = 0;
		uint32_t m_vertices_byte_offset = 0;
		uint32_t m_indices_byte_length = 0;
		uint32_t m_indices_byte_offset = 0;
	};

	typedef std::shared_ptr<LavyAsset> LavyAssetPtr;
}

#endif