#ifndef _VALKYRIE_WINDOW_MANAGER_H
#define _VALKYRIE_WINDOW_MANAGER_H
#include <string>
#include <memory>

namespace Valkyrie {
	class Window;
	using WindowPtr = std::shared_ptr<Window>;
	class WindowManager {
	public:
		static int initialize();
		static void close();
		static WindowManager& instance() { return *gp_window_manager; }

		virtual ~WindowManager();

		void createMainWindow(const std::string& title, int width, int height);
		WindowPtr getMainWindowPtr();

	private:
		static WindowManager* gp_window_manager;
		WindowManager();

		WindowPtr mp_main_window = nullptr;
	};
}

#endif