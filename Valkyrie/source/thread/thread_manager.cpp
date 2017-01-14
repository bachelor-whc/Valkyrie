#include "valkyrie/thread/thread_manager.h"
using namespace Valkyrie;

ThreadManager* ThreadManager::gp_thread_manager = nullptr;

int Valkyrie::ThreadManager::initialize() {
	if (gp_thread_manager != nullptr)
		return 0;
	gp_thread_manager = NEW_NT ThreadManager();
	if (gp_thread_manager == nullptr)
		return 1;
	return 0;
}

void Valkyrie::ThreadManager::close() {
	if (gp_thread_manager != nullptr) {
		delete gp_thread_manager;
		gp_thread_manager = nullptr;
	}
}

ThreadManager::ThreadManager() {
	auto hardware_count = std::thread::hardware_concurrency();
	m_thread_count = hardware_count == 0 ? 2 : hardware_count;
	m_thread_ptrs.resize(m_thread_count);
}

ThreadManager::~ThreadManager() {

}