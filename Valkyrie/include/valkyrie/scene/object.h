#ifndef _VALKYRIE_SCENE_OBJECT_H
#define _VALKYRIE_SCENE_OBJECT_H
#include <glm/glm.hpp>
#include "valkyrie/component/base.h"

namespace Valkyrie {
	namespace Scene {
		class Object : public Base {
		public:
			Object();
			virtual ~Object();

			virtual void start();
			virtual void update();
			glm::mat4 world; //WRYYYYYYY

			glm::vec3 position;
			glm::vec3 rotation;
			glm::vec3 scale;
			
		private:
			static const glm::vec3 s_x_axis;
			static const glm::vec3 s_y_axis;
			static const glm::vec3 s_z_axis;
		};
	}
}

#endif