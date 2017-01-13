#include <glm/glm.hpp>
#include "valkyrie/scene/object.h"
#include "valkyrie/component/camera.h"

using namespace ValkyrieComponent;

Camera::Camera() : 
	m_fov(glm::radians<float>(60)),
	m_ratio(1.0f),
	m_near(0.1f), 
	m_far(1000.0f),
	m_position(),
	m_look_at(),
	m_up(),
	m_perspective(),
	m_view() {
	
}

Camera::~Camera() {

}

void Camera::start() {

}

void Camera::update() {
	auto& object = objectWeakPtr.lock();
	if(object != nullptr) {
		auto& transform = object->transform;
		if (transform.dirty()) {
			enableDirtyFlag();
		}
		if (dirty()) {

			disableDirtyFlag();
		}
	}
	else {
		throw std::exception("A wrong component attachment.");
	}
}

void Camera::setFOV(float angle) {
	if(angle > 0.0f) {
		m_fov = glm::radians<float>(angle);
		enableDirtyFlag();
	}
}

void Camera::setRatio(float width, float height) {
	if(width > 0.0f && height > 0.0f) {
		m_ratio = width / height;
		enableDirtyFlag();
	}
}

void Camera::setNear(float near) {
	if (near > 0.0f) {
		m_near = near;
		enableDirtyFlag();
	}
}

void Camera::setFar(float far) {
	if (far > m_near) {
		m_far = far;
		enableDirtyFlag();
	}
}
