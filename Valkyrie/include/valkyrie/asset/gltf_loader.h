#ifndef _VALKYRIE_GLTF_LOADER_H 
#define _VALKYRIE_GLTF_LOADER_H 
#include <json.hpp>
#include "utility.h"
#include "gltf_asset.h"

namespace Valkyrie {
	class glTFLoader {
	public:
		static const char* BUFFER;
		static const char* BUFFERS;
		static const char* BUFFER_VIEW;
		static const char* BUFFER_VIEWS;
		static const char* ACCESSOR;
		static const char* ACCESSORS;
		static const char* BYTE_LENGTH;
		static const char* BYTE_OFFSET;
		static const char* BYTE_STRIDE;
		static const char* COMPONENT_TYPE;
		static const char* COUNT;
		static const char* TYPE;

		glTFLoader() {};
		virtual ~glTFLoader() {};

		glTFAssetPtr load(const std::string& filename) throw(...);
	private:
		void loadBufferDescriptions(const Valkyrie::glTFAssetPtr& asset_ptr, const JSON& json) throw(...);
		void loadBufferViewDescriptions(const Valkyrie::glTFAssetPtr& asset_ptr, const JSON& json);
		void loadAccessorDescriptions(const Valkyrie::glTFAssetPtr& asset_ptr, const JSON& json);

		std::map<std::string, std::string> m_buffer_uri_map;
		std::map<std::string, Valkyrie::MemoryChunkPtr> m_uri_memory_chunk_map;
		std::map<std::string, Valkyrie::glTFBufferPtr> m_buffer_map;
		std::map<std::string, Valkyrie::glTFBufferViewPtr> m_buffer_view_map;
		std::map<std::string, Valkyrie::glTFAccessorPtr> m_accessor_map;
	};
}

#endif