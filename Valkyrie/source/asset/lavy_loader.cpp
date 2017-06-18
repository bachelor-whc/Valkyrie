#include <glm/glm.hpp>
#include "valkyrie/asset/lavy_loader.h"
#include "valkyrie/asset/lavy_asset.h"
#include "valkyrie/asset/asset_manager.h"
using namespace Valkyrie;
using std::experimental::filesystem::path;
using std::experimental::filesystem::exists;
using std::experimental::filesystem::create_directory;
using std::experimental::filesystem::current_path;
using std::experimental::filesystem::file_size;
using std::experimental::filesystem::is_regular_file;

const char* LavyLoader::VERTICES = "vertices";
const char* LavyLoader::INDICES = "indices";
const char* LavyLoader::BYTE_LENGTH = "byteLength";
const char* LavyLoader::BYTE_OFFSET = "byteOffset";

LavyAssetPtr LavyLoader::load(const path& lavy_file_path) throw(...) {
	path lavy_json_path = lavy_file_path.u8string() + ".json";
	path lavy_bin_path = lavy_file_path.u8string() + ".bin";
	std::ifstream file(lavy_json_path);
	if (!file.is_open()) {
		std::string ex_message = "File " + lavy_json_path.filename().u8string() + " not open.";
		throw std::exception(ex_message.c_str());
	}

	JSON json;
	file >> json;
	file.close();

	LavyAssetPtr& ptr = MAKE_SHARED(LavyAsset)(json);

	try {
		loadBufferDescriptions(ptr, json);
		loadBinaryFile(ptr, lavy_bin_path);
	}
	catch(std::exception& e) {
		throw e;
	}
	return ptr;
}

void LavyLoader::loadBufferDescriptions(const LavyAssetPtr& asset_ptr, const JSON& json) {
	const auto get_byte_information = [](auto& j, uint32_t& byte_length, uint32_t& byte_offset) {
		byte_length = j[BYTE_LENGTH].get<uint32_t>();
		byte_offset = j[BYTE_OFFSET].get<uint32_t>();
	};
	auto& vertices_json = json["vertices"];
	auto& indices_json = json["indices"];
	get_byte_information(vertices_json, asset_ptr->m_vertices_byte_length, asset_ptr->m_vertices_byte_offset);
	get_byte_information(indices_json, asset_ptr->m_indices_byte_length, asset_ptr->m_indices_byte_offset);
	asset_ptr->m_indices_count = asset_ptr->m_indices_byte_length / 4;
	glm::vec3 min(
		json["bounding_box"]["min"][0].get<float>(),
		json["bounding_box"]["min"][1].get<float>(),
		json["bounding_box"]["min"][2].get<float>()
	);
	glm::vec3 max(
		json["bounding_box"]["max"][0].get<float>(),
		json["bounding_box"]["max"][1].get<float>(),
		json["bounding_box"]["max"][2].get<float>()
	);
	asset_ptr->m_bounding_box.min = min;
	asset_ptr->m_bounding_box.max = max;
}

void Valkyrie::LavyLoader::loadBinaryFile(const Valkyrie::LavyAssetPtr& asset_ptr, const path& bin_file_path) {
	auto& asset_manager = AssetManager::instance();
	asset_manager.fillMemoryFromFile(asset_ptr->m_buffer_ptr, bin_file_path);
}
