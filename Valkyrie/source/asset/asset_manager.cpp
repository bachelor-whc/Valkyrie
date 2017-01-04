#include "valkyrie/asset/asset_manager.h"
#include "common.h"
using namespace Valkyrie;

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

Valkyrie::AssetManager::AssetManager() :
	m_path(std::tr2::sys::current_path()){

}
