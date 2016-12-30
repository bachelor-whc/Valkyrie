#ifndef _VALKYRIE_GLTF_LOADER_H 
#define _VALKYRIE_GLTF_LOADER_H 
#include <json.hpp>
#include "utility.h"
#include "gltf_asset.h"

namespace Valkyrie {
	class glTFLoader {
	public:
		glTFLoader() {};
		virtual ~glTFLoader() {};

		glTFAssetPtr load(const std::string& filename);
	};
}

#endif