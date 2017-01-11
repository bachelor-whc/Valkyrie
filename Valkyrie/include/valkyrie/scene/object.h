#ifndef _VALKYRIE_SCENE_OBJECT_H
#define _VALKYRIE_SCENE_OBJECT_H
#include "valkyrie/scene/transform.h"
#include "valkyrie/component/base.h"

namespace Valkyrie {
	namespace Scene {
		class Object : public Base {
		public:
			Object();
			virtual ~Object();

			virtual void start();
			virtual void update();
			
			Transform transform;
		private:
		};
	}
}

#endif