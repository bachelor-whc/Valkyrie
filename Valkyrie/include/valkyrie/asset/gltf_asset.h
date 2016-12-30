#ifndef _VALKYRIE_GLTF_ASSET_H 
#define _VALKYRIE_GLTF_ASSET_H 
#include <json.hpp>
#include "asset.h"
#include "utility.h"
#include "valkyrie/vulkan/memory_buffer.h"

namespace Valkyrie {
	class glTFAsset : public Asset {
	public:
		glTFAsset() {};
		glTFAsset(const JSON& src);
		virtual ~glTFAsset() {};

		virtual void* getData() { return &m_json; };
		virtual uint32_t getSize() const { return 0; };
		void setJSON(const JSON& src);

		std::set<std::string> buffersSet;
		std::set<std::string> uriSet;
	private:
		JSON m_json;
		std::map <std::string, Vulkan::MemoryBufferPtr> buffers;
		//std::map <std::string, Vulkan::MemoryBufferPtr> buffers;
	};
	typedef std::shared_ptr<glTFAsset> glTFAssetPtr;
}

#endif