#ifndef _VALKYRIE_OBJECT_MANAGER_H
#define _VALKYRIE_OBJECT_MANAGER_H
#include <queue>
#include <unordered_map>
#include <random>
#include "valkyrie/base_unit.h"
#include "valkyrie/scene/object.h"
#define MAX_NUM_OF_OBJECTS 65536

namespace Valkyrie {
	class ObjectManager {
	public:
		static const int ILLEGAL_ID = -1;
		static const int ALREADY_REGISTERED = -2;
		static int initialize();
		static void close();
		static ObjectManager& instance() { return *gp_object_manager; }

		virtual ~ObjectManager();

		unsigned int acquireNextID();
		int registerObject(const Scene::ObjectPtr& ptr);
		Scene::ObjectPtr getObject(const unsigned int ID);

	private:
		static ObjectManager* gp_object_manager;
		ObjectManager();

		std::deque<unsigned int> m_unused_ID;
		std::deque<unsigned int> m_used_ID;
		std::unordered_map<unsigned int, Scene::ObjectPtr> m_table;
		std::random_device m_random_device;
		std::mt19937 m_mt19937;
	};
}

#endif