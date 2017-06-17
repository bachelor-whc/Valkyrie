#ifndef _VALKYRIE_GLTF_LOADER_H 
#define _VALKYRIE_GLTF_LOADER_H 
#include <json.hpp>
#include <filesystem>
#include "valkyrie/common.h"
#include "lavy_asset.h"

namespace Valkyrie {
	class LavyLoader {
	public:
		static const char* VERTICES;
		static const char* INDICES;
		static const char* BYTE_LENGTH;
		static const char* BYTE_OFFSET;

		LavyLoader() {};
		virtual ~LavyLoader() {};

		LavyAssetPtr load(const std::experimental::filesystem::path& lavy_file_path) throw(...);
	private:
		void loadBufferDescriptions(const Valkyrie::LavyAssetPtr& asset_ptr, const JSON& json) throw(...);
		void loadBinaryFile(const Valkyrie::LavyAssetPtr& asset_ptr, const std::experimental::filesystem::path& bin_file_path) throw(...);

		std::experimental::filesystem::path m_working_directory;
	};
}

#endif