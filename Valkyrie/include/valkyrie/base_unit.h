#ifndef _VALKYRIE_BASE_H
#define _VALKYRIE_BASE_H

namespace Valkyrie {
	class Base {
	public:
		Base() {};
		virtual ~Base() {};

		virtual void start() = 0;
		virtual void update() = 0;
	};
}

#endif