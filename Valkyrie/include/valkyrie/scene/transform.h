#ifndef _VALKYRIE_SCENE_TRANSFORM_H
#define _VALKYRIE_SCENE_TRANSFORM_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "valkyrie/base_unit.h"

namespace Valkyrie {
	namespace Scene {
		class Transform {
		public:
			enum DirtyFlag {
				TRANSLATE = 1,
				ROTATION = 2,
				SCALE = 4
			};

			Transform();
			virtual ~Transform();

			glm::mat4& getWorldMatrix();

			void setTranslate(const float x, const float y, const float z);
			/// \brief Taking degree as arguments.
			void setRotation(const float dx, const float dy, const float dz);
			void setScale(const float x, const float y, const float z);

			/// \brief Get rotation matrix.
			const glm::mat3 getRotationMatrix3() const;
			/// \brief Get rotation matrix.
			const glm::mat4 getRotationMatrix4() const;
			/// \brief Get value of translate.
			const glm::vec3& getTranslteValue() const;
			/// \brief Get value of roatation in degree.
			const glm::vec3& getRotationValue() const;
			/// \brief Get value of scale.
			const glm::vec3& getScaleValue() const;
			/// Getting reference of instance will enable dirty flag.
			/// For evaluation purpose, just use getTranslteValue()
			glm::vec3& getTranslteRef();
			/// Getting reference of instance will enable dirty flag.
			/// For evaluation purpose, just use getRotationValue()
			glm::vec3& getRotationRef();
			/// Getting reference of instance will enable dirty flag.
			/// For evaluation purpose, just use getScaleValue()
			glm::vec3& getScaleRef();

			/// Update world matrix and make orientation normalized.
			void update();

		private:
			DirtyFlags m_dirty;
			glm::mat4 m_world;
			glm::vec3 m_translate;
			glm::vec3 m_rotation;
			glm::vec3 m_scale;
		};
	}
}

#endif