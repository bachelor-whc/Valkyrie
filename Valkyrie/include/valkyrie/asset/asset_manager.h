#ifndef _VALKYRIE_ASSET_MANAGER_H
#define _VALKYRIE_ASSET_MANAGER_H
#include <filesystem>
#include "valkyrie/memory_chunk.h"

namespace Valkyrie {
	class AssetManager {
	public:
		static int initialize();
		static void close();
		static AssetManager* getGlobalAssetMangerPtr() { return gp_asset_manager; }

		virtual ~AssetManager();
		void load(std::string asset_file_name, MemoryAccess* memory);

	private:
		static AssetManager* gp_asset_manager;
		AssetManager();

		std::tr2::sys::path m_path;
	};
}

#endif