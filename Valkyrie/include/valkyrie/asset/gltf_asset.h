#ifndef _VALKYRIE_GLTF_ASSET_H 
#define _VALKYRIE_GLTF_ASSET_H 
#include "utility.h"
#include "asset.h"
#include <json.hpp>

class ValkyrieglTF : public ValkyrieAsset {
public:
	ValkyrieglTF() {};
	ValkyrieglTF(const JSON& src);
	virtual ~ValkyrieglTF() {};

	virtual void* getData() { return &m_json; };
	virtual uint32_t getSize() const { return 0; };
	void setJSON(const JSON& src);

private:
	JSON m_json;
};

typedef std::shared_ptr<ValkyrieglTF> ValkyrieglTFPtr;

#endif