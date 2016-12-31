#include "valkyrie/asset/gltf_loader.h"
#include "valkyrie/asset/gltf_asset.h"

Valkyrie::glTFAssetPtr Valkyrie::glTFLoader::load(const std::string& filename) {
	JSON json;
	std::ifstream file(filename);
	assert(file.is_open());
	file >> json;
	Valkyrie::glTFAssetPtr ptr = std::make_shared<Valkyrie::glTFAsset>(json);
	for (auto it = json["buffers"].begin(); it != json["buffers"].end(); ++it) {
		auto& j = it.value();
		ptr->bufferSet.insert(it.key());
		ptr->uriSet.insert(j["uri"].get<std::string>());
	}
	return ptr;
}
