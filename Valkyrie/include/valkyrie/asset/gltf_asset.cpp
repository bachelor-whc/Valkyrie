#include "gltf_asset.h"

ValkyrieglTF::ValkyrieglTF(const JSON& src) : m_json(src) {

}

void ValkyrieglTF::setJSON(const JSON& src) {
	m_json = src;
}
