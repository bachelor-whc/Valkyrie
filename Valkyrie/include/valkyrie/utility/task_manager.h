#ifndef _VALKYRIE_TASK_MANAGER_H
#define _VALKYRIE_TASK_MANAGER_H
#include <tbb/task_group.h>
#include <vector>

namespace Valkyrie {
	class TaskManager {
	public:
		static int initialize();
		static void close();
		static bool initialized();
		static TaskManager& instance() { return *gp_task_manager; }

		virtual ~TaskManager();

		tbb::task_group group;

	private:
		static TaskManager* gp_task_manager;
		TaskManager();

		unsigned int m_thread_numbers;
	};
}

#endif