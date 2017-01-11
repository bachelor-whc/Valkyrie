#ifndef _VALKYRIE_SCENE_TRANSFORM_H
#define _VALKYRIE_SCENE_TRANSFORM_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "valkyrie/component/base.h"

namespace Valkyrie {
	namespace Scene {
		template<typename T, typename P>
		class UpdateCheckableVec3 {
		public :
			UpdateCheckableVec3() : data() {};
			virtual ~UpdateCheckableVec3() {};
			void setX(P x) { data.x = x; dirty = true; }
			void setY(P y) { data.y = y; dirty = true; }
			void setZ(P z) { data.z = z; dirty = true; }
			T& getData() { return data; }
			void update() { dirty = false; };
			bool isDirty() { return dirty; }
		private:
			bool dirty = true;
			T data;
		};

		using ufvec3 = UpdateCheckableVec3<glm::vec3, float>;

		class Transform {
		public:
			Transform() {
				scale.setX(1.0f);
				scale.setY(1.0f);
				scale.setZ(1.0f);
			};
			virtual ~Transform() {};

			void update() {
				if (translate.isDirty() || rotation.isDirty() || scale.isDirty()) {
					m_world = glm::mat4(1.0f);
					glm::vec3 r = rotation.getData();
					r.x = glm::radians(r.x);
					r.y = glm::radians(r.y);
					r.z = glm::radians(r.z);
					glm::quat quaternion(r);
					m_world = glm::scale(m_world, scale.getData());
					m_world = glm::translate(m_world, translate.getData());
					m_world *= glm::mat4_cast(quaternion);
					translate.update();
					rotation.update();
					scale.update();
				}
			}

			glm::mat4& getWorldMatrix() {
				update();
				return m_world;
			}

			ufvec3 translate;
			ufvec3 rotation;
			ufvec3 scale;
			
		private:
			glm::mat4 m_world;
		};
	}
}

#endif