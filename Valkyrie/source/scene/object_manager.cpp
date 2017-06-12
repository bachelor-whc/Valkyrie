#include <algorithm>
#include "common.h"
#include "valkyrie/scene/object_manager.h"
using namespace Valkyrie;

ObjectManager* ObjectManager::gp_object_manager = nullptr;

int ObjectManager::initialize() {
	gp_object_manager = NEW_NT ObjectManager;
	if (gp_object_manager == nullptr)
		return 1;
	return 0;
}

void ObjectManager::close() {
	if (gp_object_manager != nullptr) {
		delete gp_object_manager;
		gp_object_manager = nullptr;
	}
}

ObjectManager::ObjectManager() : 
	m_unused_ID(MAX_NUM_OF_OBJECTS),
	m_used_ID(),
	m_table(),
	m_random_device(),
	m_mt19937(m_random_device()) {
	int n = 0;
	std::generate(m_unused_ID.begin(), m_unused_ID.end(), [&n]() { return n++; });
	std::shuffle(m_unused_ID.begin(), m_unused_ID.end(), m_mt19937);
}

ObjectManager::~ObjectManager() {

}