#include "valkyrie/component/component_attacher.h"
#include "valkyrie/component/component.h"
#include "valkyrie/component/camera.h"
#include "valkyrie/scene/object.h"
#include "valkyrie/scene/camera.h"
using namespace Valkyrie;

ComponentAttacher::ComponentAttacher() {

}

ComponentAttacher::~ComponentAttacher() {

}

void ComponentAttacher::attachComponent(const Scene::ObjectPtr& object_ptr, const ValkyrieComponent::ComponentPtr& component_ptr) {
	if (object_ptr != nullptr && component_ptr != nullptr) {
		std::lock_guard<std::mutex> lock(m_attach_component);
		component_ptr->objectWeakPtr = object_ptr;
		auto&& component_name = component_ptr->getName();
		object_ptr->m_component_ptrs.push_back(component_ptr);
		object_ptr->m_component_index[component_name] = object_ptr->m_component_ptrs.size() - 1;
	}
}

void ComponentAttacher::attachComponent(const Scene::CameraPtr& camera_ptr, const ValkyrieComponent::CameraComponentPtr& component_ptr) {
	std::lock_guard<std::mutex> lock(m_attach_component);
	component_ptr->objectWeakPtr = camera_ptr;
	camera_ptr->m_camera_component_ptr = component_ptr;
}
