#ifndef _VALKYRIE_SCENE_TRANSFORM_H
#define _VALKYRIE_SCENE_TRANSFORM_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "valkyrie/base_unit.h"

namespace Valkyrie {
	namespace Scene {
		class Transform : 
			public DirtyFlag {
		public:
			Transform();
			virtual ~Transform();

			glm::mat4& getWorldMatrix();

			void setTranslate(const float x, const float y, const float z);
			
			/// Taking degree as arguments.
			void setRotation(const float dx, const float dy, const float dz);
			void setScale(const float x, const float y, const float z);
			const glm::vec3& getTranslteValue() const;
			const glm::vec3& getRotationValue() const;
			const glm::vec3& getScaleValue() const;
			glm::vec3& getTranslteRef();
			glm::vec3& getRotationRef();
			glm::vec3& getScaleRef();

		private:
			void update();

			glm::mat4 m_world;
			glm::vec3 m_translate;
			glm::vec3 m_rotation;
			glm::vec3 m_scale;
		};
	}
}

#endif