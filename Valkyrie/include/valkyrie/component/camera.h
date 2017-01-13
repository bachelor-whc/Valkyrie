#ifndef _VALKYRIECOMPONENT_CAMERA_H
#define _VALKYRIECOMPONENT_CAMERA_H
#include <glm/glm.hpp>
#include "valkyrie/base_unit.h"
#include "valkyrie/component/component.h"

namespace ValkyrieComponent {
	class Camera : 
		public Component,
		public Valkyrie::DirtyFlag {
	public:
		Camera();
		virtual ~Camera();

		virtual void start();
		virtual void update();

		///
		/// \brief Set FOV.
		///
		/// Unacceptable arugment will be ignored.
		/// @param angle should be in degree and greater then 0.
		///
		void setFOV(float angle);
		///
		/// \brief Set Ratio.
		///
		/// Unacceptable arugment will be ignored.
		/// @param width, height should be in degree and greater then 0.
		///
		void setRatio(float width, float height);
		///
		/// \brief Set near.
		///
		/// Unacceptable arugment will be ignored.
		/// @param near should be greater then 0.
		///
		void setNear(float near);
		///
		/// \brief Set far.
		///
		/// Unacceptable arugment will be ignored.
		/// @param far should be greater then near.
		///
		void setFar(float far);

	private:
		float m_fov;
		float m_ratio;
		float m_near;
		float m_far;

		glm::vec3 m_position;
		glm::vec3 m_look_at;
		glm::vec3 m_up;
		glm::mat4 m_perspective;
		glm::mat4 m_view;
	};
}

#endif