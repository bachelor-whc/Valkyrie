#ifndef _VALKYRIE_OBJECT_MANAGER_H
#define _VALKYRIE_OBJECT_MANAGER_H
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <mutex>
#include "valkyrie/base_unit.h"
#include "valkyrie/scene/object.h"
#define DEFAULT_NUM_OF_OBJECTS 65536

namespace Valkyrie {
	class ObjectManager {
	public:
		static const int ILLEGAL_ID = -1;
		static const int ALREADY_REGISTERED = -2;
		static int initialize();
		static void close();
		static bool initialized();
		static ObjectManager& instance() { return *gp_object_manager; }

		virtual ~ObjectManager();

		unsigned int acquireNextID();
		int registerObject(const Scene::ObjectPtr& ptr);
		void unregisterObject(const unsigned int ID);
		Scene::ObjectPtr getObject(const unsigned int ID) const;
		inline std::mt19937 getRandomGenerator() { return m_mt19937; };

	private:
		static ObjectManager* gp_object_manager;
		ObjectManager();

		void expandQueue();
		void setTable(const unsigned int ID, const Scene::ObjectPtr& ptr);
		void returnID(const unsigned int ID);

		unsigned short m_resize_times = 0;
		std::mutex m_registration_mutex;
		std::mutex m_acquire_mutex;
		std::deque<unsigned int> m_unused_ID;
		std::unordered_set<unsigned int> m_used_ID;
		std::unordered_map<unsigned int, Scene::ObjectPtr> m_table;
		std::random_device m_random_device;
		std::mt19937 m_mt19937;
	};
}

#endif