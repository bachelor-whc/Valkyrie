#include "valkyrie/asset/gltf_loader.h"
#include "valkyrie/asset/gltf_asset.h"

Valkyrie::glTFAssetPtr Valkyrie::glTFLoader::load(const std::string& filename) {
	JSON json;
	std::ifstream file(filename);
	assert(file.is_open());
	file >> json;
	Valkyrie::glTFAssetPtr ptr = std::make_shared<Valkyrie::glTFAsset>(json);
	return ptr;
}
