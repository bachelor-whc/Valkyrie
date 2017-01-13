#include "valkyrie/scene/transform.h"

using namespace Valkyrie::Scene;

Transform::Transform() :
	m_translate(),
	m_rotation(),
	m_scale(1.0f, 1.0f, 1.0f) {
	
}

Transform::~Transform() {

}

void Transform::update() {
	if (dirty()) {
		m_world = glm::mat4(1.0f);
		glm::quat quaternion(m_rotation);
		m_world = glm::scale(m_world, m_scale);
		m_world = glm::translate(m_world, m_translate);
		m_world *= glm::mat4_cast(quaternion);
		disableDirtyFlag();
	}
}

glm::mat4& Transform::getWorldMatrix() {
	update();
	return m_world;
}

void Transform::setTranslate(const float x, const float y, const float z) {
	m_translate.x = x;
	m_translate.y = y;
	m_translate.z = z;
	enableDirtyFlag();
}

void Transform::setRotation(const float dx, const float dy, const float dz) {
	m_rotation.x = glm::radians<float>(dx);
	m_rotation.y = glm::radians<float>(dy);
	m_rotation.z = glm::radians<float>(dz);
	enableDirtyFlag();
}

void Transform::setScale(const float x, const float y, const float z) {
	if(x > 0.0f && y > 0.0f && z > 0.0f) {
		m_scale.x = x;
		m_scale.y = y;
		m_scale.z = z;
		enableDirtyFlag();
	}
}

const glm::vec3& Transform::getTranslteValue() const {
	return m_translate;
}

const glm::vec3& Transform::getRotationValue() const {
	return m_rotation;
}

const glm::vec3& Transform::getScaleValue() const {
	return m_scale;
}

glm::vec3& Transform::getTranslteRef() {
	enableDirtyFlag();
	return m_translate;
}

glm::vec3& Transform::getRotationRef() {
	enableDirtyFlag();
	return m_rotation;
}

glm::vec3& Transform::getScaleRef() {
	enableDirtyFlag();
	return m_scale;
}