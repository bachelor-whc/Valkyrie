#include "valkyrie/base_unit.h"

using namespace Valkyrie;


void DirtyFlags::reset() {
	m_dirty = 0;
}

void DirtyFlags::enableDirtyFlag(const unsigned int flag) {
	m_dirty |= flag;
}

void DirtyFlags::disableDirtyFlag(const unsigned int flag) {
	m_dirty = m_dirty & (~flag);
}

bool DirtyFlags::dirty() {
	return m_dirty > 0;
}

bool DirtyFlags::dirtyAt(const unsigned int flag) {
	return (m_dirty & flag) > 0;
	
}