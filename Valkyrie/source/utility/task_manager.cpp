#include <thread>
#include "valkyrie/utility/task_manager.h"
#include "common.h"
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

TaskManager::TaskManager() : 
	m_thread_numbers(0),
	group() {
	m_thread_numbers = std::thread::hardware_concurrency() * 2 + 1;
}

TaskManager::~TaskManager() {

}
