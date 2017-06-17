#ifndef _VALKYRIE_BASE_H
#define _VALKYRIE_BASE_H
#include <memory>
#include <climits>

namespace ValkyrieFactory {
	class ObjectFactory;
}

namespace Valkyrie {
	class Base {
		friend class ValkyrieFactory::ObjectFactory;

	public:
		Base() {}
		virtual ~Base() {}

		virtual void start() = 0;
		virtual void update() = 0;

		unsigned int getID();

	private:
		unsigned int m_ID = UINT_MAX;
	};
	using BasePtr = std::shared_ptr<Base>;

	class DirtyFlagView {
	public:
		DirtyFlagView() {}
		virtual ~DirtyFlagView() {}

		virtual bool dirty() = 0;
		virtual bool dirtyAt(const unsigned int flag) = 0;
	};

	class DirtyFlags {
	public:
		DirtyFlags() {}
		virtual ~DirtyFlags() {}

		void reset();
		void enableDirtyFlag(const unsigned int flag);
		void disableDirtyFlag(const unsigned int flag);
		bool dirty() const;
		bool dirtyAt(const unsigned int flag) const;
	private:
		unsigned int m_dirty = 0;
	};
}

#endif