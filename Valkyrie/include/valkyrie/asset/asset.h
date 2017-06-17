#ifndef _VALKYRIE_ASSET_H 
#define _VALKYRIE_ASSET_H 
#include "valkyrie/common.h"

namespace Valkyrie {
	class Asset {
	public:
		Asset() {};
		virtual ~Asset() {};
	};

	typedef std::shared_ptr<Asset> AssetPtr;
}

#endif