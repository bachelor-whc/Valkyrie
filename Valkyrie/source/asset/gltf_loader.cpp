#include "valkyrie/asset/gltf_loader.h"
#include "valkyrie/asset/gltf_asset.h"

Valkyrie::glTFAssetPtr Valkyrie::glTFLoader::load(const std::string& filename) {
	JSON json;
	std::ifstream file(filename);
	assert(file.is_open());
	file >> json;
	Valkyrie::glTFAssetPtr& ptr = std::make_shared<Valkyrie::glTFAsset>(json);
	std::map<std::string, std::string> buffer_uri_map;
	std::map<std::string, Valkyrie::MemoryChunkPtr> uri_memory_chunk_map;
	std::map<std::string, Valkyrie::glTFBufferPtr> buffer_map;
	for (auto it = json["buffers"].begin(); it != json["buffers"].end(); ++it) {
		auto& j = it.value();
		auto buffer_name = it.key();
		ptr->bufferSet.insert(buffer_name);

		std::string uri = j["uri"].get<std::string>();
		ptr->uriSet.insert(uri);
		buffer_uri_map[buffer_name] = uri;

		uint32_t byte_length = j["byteLength"].get<uint32_t>();
		auto& memory_chunk_ptr = MAKE_SHARED(Valkyrie::MemoryChunk)();
		uri_memory_chunk_map[uri] = memory_chunk_ptr;
		memory_chunk_ptr->allocate(byte_length);

		buffer_map[buffer_name] = MAKE_SHARED(Valkyrie::glTFBuffer)(memory_chunk_ptr);
	}
	for (auto it = json["buffers"].begin(); it != json["buffers"].end(); ++it) {
		auto& j = it.value();
		auto buffer_name = it.key();
		ptr->bufferSet.insert(buffer_name);

		std::string uri = j["uri"].get<std::string>();
		ptr->uriSet.insert(uri);
		buffer_uri_map[buffer_name] = uri;

		uint32_t byte_length = j["byteLength"].get<uint32_t>();
		auto& memory_chunk_ptr = MAKE_SHARED(Valkyrie::MemoryChunk)();
		uri_memory_chunk_map[uri] = memory_chunk_ptr;
		memory_chunk_ptr->allocate(byte_length);

		buffer_map[buffer_name] = MAKE_SHARED(Valkyrie::glTFBuffer)(memory_chunk_ptr);
	}

	return ptr;
}
