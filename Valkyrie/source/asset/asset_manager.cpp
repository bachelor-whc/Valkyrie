#include "valkyrie/common.h"
#include "valkyrie/asset/asset_manager.h"
#include "valkyrie/asset/mesh_asset.h"
#include "valkyrie/factory/mesh.h"
#include "valkyrie/utility/memory_chunk.h"
using namespace Valkyrie;
using std::experimental::filesystem::path;
using std::experimental::filesystem::exists;
using std::experimental::filesystem::create_directory;
using std::experimental::filesystem::current_path;
using std::experimental::filesystem::file_size;
using std::experimental::filesystem::is_regular_file;

AssetManager* AssetManager::gp_asset_manager = nullptr;

int AssetManager::initialize() {
	if (gp_asset_manager != nullptr)
		return 0;
	gp_asset_manager = NEW_NT AssetManager();
	if (gp_asset_manager == nullptr)
		return 1;
	return 0;
}

void AssetManager::close() {
	if (gp_asset_manager != nullptr)
		delete gp_asset_manager;
	gp_asset_manager = nullptr;
}

bool AssetManager::initialized() {
	return gp_asset_manager != nullptr;
}

AssetManager::~AssetManager() {

}

void AssetManager::load(path file_path) throw(...) {
	if (file_path.is_relative()) {
		file_path = m_path / file_path;
	}
	if (file_path.extension() == ".lavy") {
		auto& ptrs = m_lavy_loader.load(file_path);
		for(auto ptr : ptrs) {
			auto& factory = ValkyrieFactory::MeshFactory::instance();
			m_mesh_map[ptr->getName()] = factory.createMesh(ptr);
		}
	}
	else if (exists(file_path) && is_regular_file(file_path)) {
		MemoryChunkPtr ptr = MAKE_SHARED(MemoryChunk)();
		fillMemoryFromFile(ptr, file_path);
		m_asset_map[file_path.u8string()] = ptr;
	}
	else {
		std::string ex_message = file_path.filename().u8string() + " is not avaliable.";
		throw std::exception(ex_message.c_str());
	}
}

AssetManager::AssetManager() :
	m_lavy_loader() {
	m_path = current_path() / "assets";
	if (!exists(m_path)) {
		create_directory(m_path);
	}
}

void AssetManager::fillMemoryFromFile(MemoryChunkPtr& ptr, const std::experimental::filesystem::path& abs_path) throw(...) {
	const auto&& key = abs_path.u8string();
	if (m_memory_chunk_map.count(key) > 0) {
		ptr = m_memory_chunk_map[key];
	}
	else {
		ptr = MAKE_SHARED(MemoryChunk)();
		uint32_t size = file_size(abs_path);
		std::ifstream file_stream(abs_path, std::ios::binary | std::ios::binary);
		if (!file_stream.is_open()) {
			std::string ex_message = abs_path.filename().u8string() + " is not avaliable.";
			throw std::exception(ex_message.c_str());
		}
		if (!ptr->allocated()) {
			ptr->allocate(size);
		}
		else if (ptr->getSize() < size) {
			std::string ex_message = "Memory size is smaller than " + abs_path.filename().u8string();
			throw std::exception(ex_message.c_str());
		}
		std::filebuf* p_buffer = file_stream.rdbuf();
		p_buffer->sgetn((char*)ptr->getData(), size);
		file_stream.close();
		ptr->setFlags(MemoryAccess::READY);
		m_memory_chunk_map[key] = ptr;
	}
}

AssetPtr AssetManager::getAsset(std::string relative_path) {
	path& abs_path = m_path / relative_path;
	if (m_asset_map.count(abs_path.u8string()) > 0)
		return m_asset_map[abs_path.u8string()];
	else
		return nullptr;
}

MeshPtr AssetManager::getMesh(std::string mesh_name) {
	if (m_mesh_map.count(mesh_name) > 0)
		return m_mesh_map[mesh_name];
	else
		return nullptr;
}
