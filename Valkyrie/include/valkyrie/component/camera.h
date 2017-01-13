#ifndef _VALKYRIECOMPONENT_CAMERA_H
#define _VALKYRIECOMPONENT_CAMERA_H
#include "valkyrie/base_unit.h"
#include "valkyrie/component/component.h"

namespace ValkyrieComponent {
	class Camera : public Component {
	public:
		Camera();
		virtual ~Camera();

		virtual void start();
		virtual void update();

		void setFOV(float angle);
		void setRatio(float width, float height);
		void setNear(float near);
		void setFar(float far);

	private:
		float m_fov;
		float m_ratio;
		float m_near;
		float m_far;
	};
}

#endif