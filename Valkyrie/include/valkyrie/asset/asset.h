#ifndef _VALKYRIE_ASSET_H 
#define _VALKYRIE_ASSET_H 
#include "utility.h"

namespace Valkyrie {
	class Asset {
	public:
		Asset() {};
		virtual ~Asset() {};

		virtual void* getData() = 0;
		virtual uint32_t getSize() const = 0;
	};
}

#endif