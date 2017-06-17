#include <algorithm>
#include <limits>
#include "valkyrie/common.h"
#include "valkyrie/scene/object_manager.h"
using namespace Valkyrie;

ObjectManager* ObjectManager::gp_object_manager = nullptr;

int ObjectManager::initialize() {
	gp_object_manager = NEW_NT ObjectManager;
	if (gp_object_manager == nullptr)
		return 1;
	return 0;
}

bool ObjectManager::initialized() {
	return gp_object_manager != nullptr;
}

void ObjectManager::close() {
	if (gp_object_manager != nullptr) {
		delete gp_object_manager;
		gp_object_manager = nullptr;
	}
}

ObjectManager::ObjectManager() : 
	m_unused_ID(DEFAULT_NUM_OF_OBJECTS),
	m_used_ID(),
	m_table(),
	m_random_device(),
	m_mt19937(m_random_device()) {
	++m_resize_times;
	unsigned int n = 0;
	std::generate(m_unused_ID.begin(), m_unused_ID.end(), [&n]() { return n++; });
	std::shuffle(m_unused_ID.begin(), m_unused_ID.end(), m_mt19937);
}

void ObjectManager::expandQueue() {
	if (m_unused_ID.empty()) {
		unsigned int n = m_resize_times * DEFAULT_NUM_OF_OBJECTS;
		m_resize_times += 1;
		m_unused_ID.resize(DEFAULT_NUM_OF_OBJECTS);
		std::generate(m_unused_ID.begin(), m_unused_ID.end(), [&n]() { return n++; });
	}
}

void ObjectManager::setTable(const unsigned int ID, const Scene::ObjectPtr& ptr) {
	std::lock_guard<std::mutex> lock(m_registration_mutex);
	if (ptr == nullptr && m_table.count(ID) > 0) {
		m_table.erase(ID);
	}
	else if(ptr != nullptr) {
		m_table[ptr->getID()] = ptr;
	}
}

ObjectManager::~ObjectManager() {

}

unsigned int ObjectManager::acquireNextID() {
	std::lock_guard<std::mutex> lock(m_acquire_mutex);
	if (m_unused_ID.empty())
		expandQueue();
	auto result = m_unused_ID.front();
	m_unused_ID.pop_front();
	m_used_ID.insert(result);
	return result;
}

int Valkyrie::ObjectManager::registerObject(const Scene::ObjectPtr& ptr) {
	if (ptr->getID() == std::numeric_limits<unsigned int>::max()) {
		return ILLEGAL_ID;
	}
	else if (m_table.count(ptr->getID()) != 0) {
		return ALREADY_REGISTERED;
	}
	setTable(ptr->getID(), ptr);
	return 0;
}

void ObjectManager::unregisterObject(const unsigned int ID) {
	setTable(ID, nullptr);
	returnID(ID);
}

void ObjectManager::returnID(const unsigned int ID) {
	std::lock_guard<std::mutex> lock(m_acquire_mutex);
	if(m_used_ID.count(ID) > 0) {
		m_used_ID.erase(ID);
		m_unused_ID.push_back(ID);
	}
}

Scene::ObjectPtr ObjectManager::getObjectPtr(const unsigned int ID) const {
	const auto& result = m_table.find(ID);
	if (result == m_table.end())
		return nullptr;
	else
		return result->second;
}
