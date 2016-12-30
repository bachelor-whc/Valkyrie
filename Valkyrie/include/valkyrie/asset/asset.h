#ifndef _VALKYRIE_ASSET_H 
#define _VALKYRIE_ASSET_H 
#include "utility.h"

class ValkyrieAsset {
public:
	ValkyrieAsset() {};
	virtual ~ValkyrieAsset() {};

	virtual void* getData() = 0;
	virtual uint32_t getSize() const = 0;
};

#endif