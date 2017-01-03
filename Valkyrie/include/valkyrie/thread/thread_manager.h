#ifndef _VALKYRIE_THREAD_MANAGER_H 
#define _VALKYRIE_THREAD_MANAGER_H 
#include <thread>
#include "common.h"

namespace Valkyrie {
	class ThreadManager {
	public:
		ThreadManager();
		virtual ~ThreadManager();

		inline unsigned int getThreadCount() { return m_thread_count; }

	private:
		unsigned int m_thread_count;
	};
}

#endif