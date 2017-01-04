#ifndef _VALKYRIE_THREAD_MANAGER_H 
#define _VALKYRIE_THREAD_MANAGER_H 
#include "valkyrie/thread/thread.h"
#include "common.h"

namespace Valkyrie {
	class ThreadManager {
	public:
		static int initialize();
		static void close();
		static ThreadManager* getGlobalThreadManagerPtr() { return gp_thread_manager; }
		
		virtual ~ThreadManager();
		inline unsigned int getThreadCount() { return m_thread_count; }

	private:
		static ThreadManager* gp_thread_manager;
		ThreadManager();

		unsigned int m_thread_count;
		const unsigned short LOGIC = 0;
		const unsigned short GRAPHICS = 1;
		std::vector<ThreadPtr> m_thread_ptrs;
	};
}

#endif