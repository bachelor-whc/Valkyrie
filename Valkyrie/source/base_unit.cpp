#include "valkyrie/base_unit.h"

using namespace Valkyrie;

void DirtyFlag::enableDirtyFlag() {
	m_dirty = true;
}

void DirtyFlag::disableDirtyFlag() {
	m_dirty = false;
}

bool Valkyrie::DirtyFlag::dirty() {
	return m_dirty;
}
