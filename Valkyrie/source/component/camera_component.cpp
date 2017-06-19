#include <glm/glm.hpp>
#include "valkyrie/scene/object.h"
#include "valkyrie/component/camera.h"

using namespace ValkyrieComponent;
using TransformFlag = Valkyrie::Scene::Transform::DirtyFlag;

Camera::Camera() :
	Camera(60, 1.0f) {
	
}

Camera::Camera(float fov, float ratio, float near, float far) : m_dirty(),
	m_fov(glm::radians<float>(fov)),
	m_ratio(ratio),
	m_near(near),
	m_far(far),
	m_orientation(0.0f, 0.0f, -1.0f),
	m_up(0.0f, 1.0f, 0.0f),
	m_perspective(),
	m_view() {
	m_dirty.enableDirtyFlag(VIEW);
	m_dirty.enableDirtyFlag(PERSPECTIVE);
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
		frustum.update(m_perspective * m_view);
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

void Camera::setNear(float _near) {
	if (_near > 0.0f) {
		m_near = _near;
		m_dirty.enableDirtyFlag(PERSPECTIVE);
	}
}

void Camera::setFar(float _far) {
	if (_far > m_near) {
		m_far = _far;
		m_dirty.enableDirtyFlag(PERSPECTIVE);
	}
}

const glm::mat4& Camera::getPerspective() const {
	return m_perspective;
}

const glm::mat4& Camera::getView() const {
	return m_view;
}

Camera::Frustum::Frustum() {
}

Camera::Frustum::~Frustum() {
}

bool Camera::Frustum::checkPosition(const glm::vec3& position) {
	for (auto i = 0; i < m_size_of_planes; ++i) {
		glm::vec4 p4(position, 1.0f);
		const float&& distance = glm::dot(m_planes[i], p4);
		if(distance < 0.0f)
			return false;
	}
	return true;
}

void Camera::Frustum::update(const glm::mat4&& view_projection) {
	m_planes[LEFT].x = view_projection[0].w + view_projection[0].x;
	m_planes[LEFT].y = view_projection[1].w + view_projection[1].x;
	m_planes[LEFT].z = view_projection[2].w + view_projection[2].x;
	m_planes[LEFT].w = view_projection[3].w + view_projection[3].x;

	m_planes[RIGHT].x = view_projection[0].w - view_projection[0].x;
	m_planes[RIGHT].y = view_projection[1].w - view_projection[1].x;
	m_planes[RIGHT].z = view_projection[2].w - view_projection[2].x;
	m_planes[RIGHT].w = view_projection[3].w - view_projection[3].x;

	m_planes[TOP].x = view_projection[0].w - view_projection[0].y;
	m_planes[TOP].y = view_projection[1].w - view_projection[1].y;
	m_planes[TOP].z = view_projection[2].w - view_projection[2].y;
	m_planes[TOP].w = view_projection[3].w - view_projection[3].y;

	m_planes[BOTTOM].x = view_projection[0].w + view_projection[0].y;
	m_planes[BOTTOM].y = view_projection[1].w + view_projection[1].y;
	m_planes[BOTTOM].z = view_projection[2].w + view_projection[2].y;
	m_planes[BOTTOM].w = view_projection[3].w + view_projection[3].y;

	m_planes[BACK].x = view_projection[0].w + view_projection[0].z;
	m_planes[BACK].y = view_projection[1].w + view_projection[1].z;
	m_planes[BACK].z = view_projection[2].w + view_projection[2].z;
	m_planes[BACK].w = view_projection[3].w + view_projection[3].z;

	m_planes[FRONT].x = view_projection[0].w - view_projection[0].z;
	m_planes[FRONT].y = view_projection[1].w - view_projection[1].z;
	m_planes[FRONT].z = view_projection[2].w - view_projection[2].z;
	m_planes[FRONT].w = view_projection[3].w - view_projection[3].z;

	for (auto i = 0; i < m_size_of_planes; i++) {
		m_planes[i] = glm::normalize(m_planes[i]);
	}
}
