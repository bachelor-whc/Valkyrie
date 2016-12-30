#ifndef _VALKYRIE_GLTF_ASSET_H 
#define _VALKYRIE_GLTF_ASSET_H 
#include "utility.h"
#include "asset.h"
#include <json.hpp>

namespace Valkyrie {
	class glTFAsset : public Asset {
	public:
		glTFAsset() {};
		glTFAsset(const JSON& src);
		virtual ~glTFAsset() {};

		virtual void* getData() { return &m_json; };
		virtual uint32_t getSize() const { return 0; };
		void setJSON(const JSON& src);

	private:
		JSON m_json;
	};
	typedef std::shared_ptr<glTFAsset> glTFAssetPtr;
}

#endif