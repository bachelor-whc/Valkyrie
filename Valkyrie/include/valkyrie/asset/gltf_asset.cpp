#include "gltf_asset.h"

Valkyrie::glTFAsset::glTFAsset(const JSON& src) : m_json(src) {

}

void Valkyrie::glTFAsset::setJSON(const JSON& src) {
	m_json = src;
}
