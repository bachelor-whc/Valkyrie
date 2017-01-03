#include "valkyrie/thread/thread_manager.h"
using namespace Valkyrie;

ThreadManager::ThreadManager() {
	m_thread_count = std::thread::hardware_concurrency();
}

ThreadManager::~ThreadManager() {

}