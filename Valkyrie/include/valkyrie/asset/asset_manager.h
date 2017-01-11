#ifndef _VALKYRIE_ASSET_MANAGER_H
#define _VALKYRIE_ASSET_MANAGER_H
#include <filesystem>
#include "valkyrie/asset/asset.h"
#include "valkyrie/asset/lavy_loader.h"
#include "valkyrie/memory_chunk.h"

namespace Valkyrie {
	class AssetManager {
	public:
		static int initialize();
		static void close();
		static AssetManager* getGlobalAssetMangerPtr() { return gp_asset_manager; }

		virtual ~AssetManager();

		void load(std::experimental::filesystem::path file_path) throw(...);
		void fillMemoryFromFile(MemoryChunkPtr& ptr, const std::experimental::filesystem::path& abs_path) throw(...);
		std::experimental::filesystem::path getAssetsDirectory() { return m_path; };
		AssetPtr getAsset(std::string relative_path);

	private:
		static AssetManager* gp_asset_manager;
		AssetManager();

		std::experimental::filesystem::path m_path;
		std::map <std::string, AssetPtr> m_asset_map;
		LavyLoader m_lavy_loader;
	};
}

#endif