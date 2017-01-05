#include <glm/glm.hpp>
#include "valkyrie/asset/gltf_loader.h"
#include "valkyrie/asset/gltf_asset.h"
using namespace Valkyrie;

const char* glTFLoader::BUFFER = "buffer";
const char* glTFLoader::BUFFERS = "buffers";
const char* glTFLoader::BUFFER_VIEW = "bufferView";
const char* glTFLoader::BUFFER_VIEWS = "bufferViews";
const char* glTFLoader::ACCESSOR = "accessor";
const char* glTFLoader::ACCESSORS = "accessors";
const char* glTFLoader::BYTE_LENGTH = "byteLength";
const char* glTFLoader::BYTE_OFFSET = "byteOffset";
const char* glTFLoader::BYTE_STRIDE = "byteStride";
const char* glTFLoader::COMPONENT_TYPE = "componentType";
const char* glTFLoader::COUNT = "count";
const char* glTFLoader::TYPE = "type";

glTFAssetPtr glTFLoader::load(const std::string& filename) throw(...) {
	JSON json;
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::string ex_message = "File " + filename + " not open.";
		throw std::exception(ex_message.c_str());
	}
	file >> json;
	file.close();
	glTFAssetPtr& ptr = MAKE_SHARED(glTFAsset)(json);
	
	m_buffer_uri_map.clear();
	m_uri_memory_chunk_map.clear();
	m_buffer_view_map.clear();
	m_accessor_map.clear();

	try {
		loadBufferDescriptions(ptr, json);
		loadBufferViewDescriptions(ptr, json);
		loadAccessorDescriptions(ptr, json);
	}
	catch(std::exception& e) {
		throw e;
	}
	return ptr;
}

void glTFLoader::loadBufferDescriptions(const glTFAssetPtr& asset_ptr, const JSON& json) {
	for (auto it = json[BUFFERS].begin(); it != json[BUFFERS].end(); ++it) {
		auto& j = it.value();
		auto buffer_name = it.key();
		asset_ptr->bufferSet.insert(buffer_name);

		std::string uri = j["uri"].get<std::string>();
		asset_ptr->uriSet.insert(uri);
		m_buffer_uri_map[buffer_name] = uri;

		uint32_t byte_length;

		if (j.count(BYTE_LENGTH) > 0) {
			byte_length = j[BYTE_LENGTH].get<uint32_t>();
		}
		else {
			std::ifstream file(uri, std::ifstream::binary);
			if (!file.is_open()) {
				std::string ex_message = "File " + uri + " not found.";
				throw std::exception(ex_message.c_str());
			}

			file.seekg(0, file.end);
			byte_length = file.tellg();
			file.close();
		}
		auto& memory_chunk_ptr = MAKE_SHARED(MemoryChunk)();
		m_uri_memory_chunk_map[uri] = memory_chunk_ptr;
		memory_chunk_ptr->allocate(byte_length);
	}
}

void glTFLoader::loadBufferViewDescriptions(const glTFAssetPtr& asset_ptr, const JSON& json) {
	for (auto it = json[BUFFER_VIEWS].begin(); it != json[BUFFER_VIEWS].end(); ++it) {
		auto& j = it.value();
		auto buffer_view_name = it.key();
		asset_ptr->bufferViewSet.insert(buffer_view_name);

		std::string buffer_name = j[BUFFER].get<std::string>();
		uint32_t byte_offset = j[BYTE_OFFSET].get<uint32_t>();
		std::string uri = m_buffer_uri_map[buffer_name];
		
		uint32_t byte_length = 0;
		if(j.count(BYTE_LENGTH))
			byte_length = j[BYTE_LENGTH].get<uint32_t>();
		
		m_buffer_view_map[buffer_view_name] = MAKE_SHARED(glTFBufferView)(m_uri_memory_chunk_map[uri], byte_length, byte_offset);
	}
}

void glTFLoader::loadAccessorDescriptions(const glTFAssetPtr& asset_ptr, const JSON& json) {
	for (auto it = json[ACCESSORS].begin(); it != json[ACCESSORS].end(); ++it) {
		auto& j = it.value();
		auto accessor_name = it.key();
		asset_ptr->accessorSet.insert(accessor_name);

		std::string buffer_view_name = j[BUFFER_VIEW].get<std::string>();
		auto& view_ptr = m_buffer_view_map[buffer_view_name];
		uint32_t byte_offset = j[BYTE_OFFSET].get<uint32_t>();
		uint32_t count = j[COUNT].get<uint32_t>();
		uint32_t byte_stride = 0;
		if (j.count(BYTE_STRIDE))
			byte_stride = j[BYTE_STRIDE].get<uint32_t>();

		GLTF_COMPONENT_TYPE component_type = (GLTF_COMPONENT_TYPE)j[COMPONENT_TYPE].get<uint32_t>();
		GLTF_TYPE type;

		GAPIAttributeSupportPtr gaas_ptr;
		std::string type_str = j[TYPE].get<std::string>();

#define PARAMETERS view_ptr, byte_offset, byte_stride, count
		if (type_str == "SCALAR" && component_type == UNSIGNED_SHORT) {
			gaas_ptr = MAKE_SHARED(GrpahicsAPIAttribute<uint16_t>)(PARAMETERS);
			type = SCALAR;
		}
		else if (type_str == "VEC2") {
			gaas_ptr = MAKE_SHARED(GrpahicsAPIAttribute<glm::vec2>)(PARAMETERS);
			type = VEC2;
		}
		else if (type_str == "VEC3") {
			gaas_ptr = MAKE_SHARED(GrpahicsAPIAttribute<glm::vec3>)(PARAMETERS);
			type = VEC3;
		}
		else if (type_str == "VEC4") {
			gaas_ptr = MAKE_SHARED(GrpahicsAPIAttribute<glm::vec4>)(PARAMETERS);
			type = VEC4;
		}
		else if (type_str == "MAT2") {
			gaas_ptr = MAKE_SHARED(GrpahicsAPIAttribute<glm::mat2>)(PARAMETERS);
			type = MAT2;
		}
		else if (type_str == "MAT3") {
			gaas_ptr = MAKE_SHARED(GrpahicsAPIAttribute<glm::mat3>)(PARAMETERS);
			type = MAT3;
		}
		else if (type_str == "MAT4") {
			gaas_ptr = MAKE_SHARED(GrpahicsAPIAttribute<glm::mat4>)(PARAMETERS);
			type = MAT4;
		}
#undef PARAMETERS
		m_accessor_map[accessor_name] = MAKE_SHARED(glTFAccessor)(gaas_ptr, type, component_type);
	}
}
