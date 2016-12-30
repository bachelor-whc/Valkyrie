#ifndef _VALKYRIE_GLTF_LOADER_H 
#define _VALKYRIE_GLTF_LOADER_H 
#include <json.hpp>
#include "utility.h"
#include "gltf_asset.h"

class glTFLoader {
public:
	glTFLoader() {};
	virtual ~glTFLoader() {};

	ValkyrieglTFPtr load(const std::string& filename);
};

#endif