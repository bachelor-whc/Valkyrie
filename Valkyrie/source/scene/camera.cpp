#include <glm/glm.hpp>
#include "valkyrie/scene/object.h"
#include "valkyrie/component/camera.h"

using namespace ValkyrieComponent;
using TransformFlag = Valkyrie::Scene::Transform::DirtyFlag;

Camera::Camera() : 
	m_dirty(),
	m_fov(glm::radians<float>(60)),
	m_ratio(1.0f),
	m_near(0.1f), 
	m_far(1000.0f),
	m_orientation(),
	m_up(),
	m_perspective(),
	m_view() {
	m_dirty.enableDirtyFlag(VIEW);
}

Camera::~Camera() {

}

void Camera::start() {

}

void Camera::update() {
	auto& object = objectWeakPtr.lock();
	if(object != nullptr) {
		auto& transform = object->transform;
		const auto& r_matrix = transform.getRotationMatrix3();
		const auto& position = transform.getTranslteValue();
		const glm::vec3& up = m_up * r_matrix;
		const glm::vec3& orientation = m_orientation * r_matrix;
		m_view = glm::lookAt(
			position,
			position + orientation,
			up
		);
		if (m_dirty.dirtyAt(PERSPECTIVE)) {
			m_perspective = glm::perspective(m_fov, m_ratio, m_near, m_far);
			m_dirty.disableDirtyFlag(PERSPECTIVE);
		}
	}
	else {
		throw std::exception("A wrong component attachment.");
	}
}

void Camera::setFOV(float angle) {
	if(angle > 0.0f) {
		m_fov = glm::radians<float>(angle);
		m_dirty.enableDirtyFlag(PERSPECTIVE);
	}
}

void Camera::setRatio(float width, float height) {
	if(width > 0.0f && height > 0.0f) {
		m_ratio = width / height;
		m_dirty.enableDirtyFlag(PERSPECTIVE);
	}
}

void Camera::setNear(float near) {
	if (near > 0.0f) {
		m_near = near;
		m_dirty.enableDirtyFlag(PERSPECTIVE);
	}
}

void Camera::setFar(float far) {
	if (far > m_near) {
		m_far = far;
		m_dirty.enableDirtyFlag(PERSPECTIVE);
	}
}

const glm::mat4& Camera::getPerspective() const {
	return m_perspective;
}

const glm::mat4& Camera::getView() const {
	return m_view;
}