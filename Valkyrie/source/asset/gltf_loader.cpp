#include "valkyrie/asset/gltf_loader.h"
#include "valkyrie/asset/gltf_asset.h"

ValkyrieglTFPtr glTFLoader::load(const std::string& filename) {
	JSON json;
	std::ifstream file(filename);
	assert(file.is_open());
	file >> json;
	ValkyrieglTFPtr ptr = std::make_shared<ValkyrieglTF>(json);
	return ptr;
}
