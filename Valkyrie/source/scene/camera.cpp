#include <glm/glm.hpp>
#include "valkyrie/scene/object.h"
#include "valkyrie/component/camera.h"

using namespace ValkyrieComponent;

Camera::Camera() : 
	m_fov(glm::radians<float>(60)),
	m_ratio(1.0f),
	m_near(0.1f), 
	m_far(1000.0f) {

}

Camera::~Camera() {

}

void Camera::start() {

}

void Camera::update() {
	auto& object = objectWeakPtr.lock();
	object->transform.
}

void Camera::setFOV(float angle) {
	m_fov = glm::radians<float>(angle);
}

void Camera::setRatio(float width, float height) {
	m_ratio = width / height;
}

void Camera::setNear(float near) {
	if (near > 0.0f)
		m_near = near;
}

void Camera::setFar(float far) {
	if (far > m_near)
		m_far = far;
}
