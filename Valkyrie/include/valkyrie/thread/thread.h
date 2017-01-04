#ifndef _VALKYRIE_THREAD_H 
#define _VALKYRIE_THREAD_H 
#include <thread>
#include "common.h"

namespace Valkyrie {
	class Thread {
	public:
		Thread() {};
		virtual ~Thread() {};

	private:
		std::thread m_handle;
	};
	
	typedef std::shared_ptr<Thread> ThreadPtr;
}

#endif