#include "valkyrie/asset/asset_manager.h"
#include "common.h"
using namespace Valkyrie;
using std::tr2::sys::path;
using std::tr2::sys::exists;
using std::tr2::sys::create_directory;
using std::tr2::sys::current_path;
using std::tr2::sys::file_size;

AssetManager* AssetManager::gp_asset_manager = nullptr;

int Valkyrie::AssetManager::initialize() {
	if (gp_asset_manager != nullptr)
		return 0;
	gp_asset_manager = NEW_NT AssetManager();
	if (gp_asset_manager == nullptr)
		return 1;
	return 0;
}

void Valkyrie::AssetManager::close() {
	if (gp_asset_manager != nullptr)
		delete gp_asset_manager;
	gp_asset_manager = nullptr;
}

Valkyrie::AssetManager::~AssetManager() {

}

void Valkyrie::AssetManager::load(MemoryChunkPtr & memory_ptr, const std::string& asset_file_name) throw(...) {
	fillMemoryFromFile(memory_ptr, asset_file_name);
}

Valkyrie::AssetManager::AssetManager() {
	m_path = current_path() / "assets";
	if (!exists(m_path)) {
		create_directory(m_path);
	}
}

void AssetManager::fillMemoryFromFile(MemoryChunkPtr& ptr, const std::string& relative_path) throw(...) {
	path file_path = m_path / relative_path;
	if (exists(file_path) && is_regular_file(file_path)) {
		uint32_t size = file_size(file_path);
		std::ifstream file_stream(file_path, std::ios::binary | std::ios::binary);
		if (!file_stream.is_open()) {
			std::string ex_message = relative_path + " is not avaliable.";
			throw std::exception(ex_message.c_str());
		}
		if (!ptr->allocated()) {
			ptr->allocate(size);
		}
		else if (ptr->getSize() < size) {
			std::string ex_message = "Memory size is smaller than " + relative_path;
			throw std::exception(ex_message.c_str());
		}
		std::filebuf* p_buffer = file_stream.rdbuf();
		p_buffer->sgetn((char*)ptr->getData(), size);
		file_stream.close();
		ptr->setFlags(MemoryAccess::READY);
	}
	else {
		std::string ex_message = relative_path + " is not avaliable.";
		throw std::exception(ex_message.c_str());
	}
}
