#include <thread>
#include <tbb/task_scheduler_init.h>
#include "valkyrie/utility/task_manager.h"
#include "valkyrie/common.h"
using namespace Valkyrie;

TaskManager* TaskManager::gp_task_manager = nullptr;

int TaskManager::initialize() {
	gp_task_manager = NEW_NT TaskManager;
	if (gp_task_manager == nullptr)
		return 1;
	return 0;
}

void TaskManager::close() {
	if (gp_task_manager != nullptr) {
		delete gp_task_manager;
		gp_task_manager = nullptr;
	}
}

bool TaskManager::initialized() {
	return gp_task_manager != nullptr;
}

TaskManager::TaskManager() : group() {
	auto default_num_of_threads = std::thread::hardware_concurrency();
	m_num_of_threads = default_num_of_threads > 1 ? (default_num_of_threads) : 1;
	mp_init = NEW_NT tbb::task_scheduler_init(m_num_of_threads);
}

TaskManager::~TaskManager() {
	if (mp_init != nullptr) {
		delete mp_init;
	}
}