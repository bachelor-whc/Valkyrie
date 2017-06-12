#ifndef _VALKYRIE_OBJECT_MANAGER_H
#define _VALKYRIE_OBJECT_MANAGER_H
#include <queue>
#include <unordered_map>
#include <random>
#include "valkyrie/base_unit.h"
#define MAX_NUM_OF_OBJECTS 65536

namespace Valkyrie {
	class ObjectManager {
	public:
		static int initialize();
		static void close();
		static ObjectManager& instance() { return *gp_object_manager; }

		virtual ~ObjectManager();

	private:
		static ObjectManager* gp_object_manager;
		ObjectManager();

		std::deque<unsigned int> m_unused_ID;
		std::deque<unsigned int> m_used_ID;
		std::unordered_map<unsigned int, Base*> m_table;
		std::random_device m_random_device;
		std::mt19937 m_mt19937;
	};
}

#endif