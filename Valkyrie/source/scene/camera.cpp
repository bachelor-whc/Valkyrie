#include "valkyrie/scene/camera.h"
using namespace Valkyrie::Scene;

Camera::Camera() : m_camera_component_ptr(nullptr) {

}

Camera::~Camera() {

}

void Camera::start() {
	Object::start();
}

void Camera::update() {
	Object::update();
	m_camera_component_ptr->update();
}

ValkyrieComponent::ComponentPtr Camera::getComponentPtr(const std::string& component_name) {
	if (component_name == ValkyrieComponent::Camera::getCameraComponentName())
		return m_camera_component_ptr;
	return Object::getComponentPtr(component_name);
}
