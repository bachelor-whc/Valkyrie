#ifndef _VALKYRIE_TASK_MANAGER_H
#define _VALKYRIE_TASK_MANAGER_H
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#include <vector>
#include <functional>

namespace Valkyrie {
	class Job {
	public:
		Job() {};
		virtual ~Job() {};

		virtual void operator()() const {};
	};

	class TaskManager {
	public:
		static int initialize();
		static void close();
		static bool initialized();
		static TaskManager& instance() { return *gp_task_manager; }

		virtual ~TaskManager();
		inline unsigned int getNumberOfThreads() { return m_num_of_threads; }

		tbb::task_group group;
	private:
		static TaskManager* gp_task_manager;
		TaskManager();

		unsigned int m_num_of_threads = 0;
		tbb::task_scheduler_init* mp_init = nullptr;
	};
}

#endif