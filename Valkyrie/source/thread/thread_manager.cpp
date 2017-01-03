#include "valkyrie/thread/thread_manager.h"
using namespace Valkyrie;

ThreadManager::ThreadManager() {
	auto hardware_count = std::thread::hardware_concurrency();
	m_thread_count = hardware_count == 0 ? 2 : hardware_count;
}

ThreadManager::~ThreadManager() {

}