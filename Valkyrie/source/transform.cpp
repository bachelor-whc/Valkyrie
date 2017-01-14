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
	if (m_dirty.dirty()) {
		m_world = glm::mat4(1.0f);
		m_world = glm::scale(m_world, m_scale);
		m_world = glm::translate(m_world, m_translate);
		m_world *= getRotationMatrix4();
		m_dirty.reset();
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
	m_dirty.enableDirtyFlag(TRANSLATE);
}

void Transform::setRotation(const float dx, const float dy, const float dz) {
	m_rotation.x = glm::radians<float>(dx);
	m_rotation.y = glm::radians<float>(dy);
	m_rotation.z = glm::radians<float>(dz);
	m_dirty.enableDirtyFlag(ROTATION);
}

void Transform::setScale(const float x, const float y, const float z) {
	if(x > 0.0f && y > 0.0f && z > 0.0f) {
		m_scale.x = x;
		m_scale.y = y;
		m_scale.z = z;
		m_dirty.enableDirtyFlag(SCALE);
	}
}

const glm::mat3 Transform::getRotationMatrix3() const {
	glm::quat quaternion(m_rotation);
	return glm::mat3_cast(quaternion);
}

const glm::mat4 Transform::getRotationMatrix4() const {
	glm::quat quaternion(m_rotation);
	return glm::mat4_cast(quaternion);
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
	m_dirty.enableDirtyFlag(TRANSLATE);
	return m_translate;
}

glm::vec3& Transform::getRotationRef() {
	m_dirty.enableDirtyFlag(ROTATION);
	return m_rotation;
}

glm::vec3& Transform::getScaleRef() {
	m_dirty.enableDirtyFlag(SCALE);
	return m_scale;
}