#ifndef _VALKYRIE_BASE_H
#define _VALKYRIE_BASE_H

namespace Valkyrie {
	class Base {
	public:
		Base() {}
		virtual ~Base() {}

		virtual void start() = 0;
		virtual void update() = 0;
	};

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