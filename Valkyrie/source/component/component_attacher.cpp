#include "valkyrie/component/component_attacher.h"
#include "valkyrie/component/component.h"
#include "valkyrie/scene/object.h"

using namespace Valkyrie;

ComponentAttacher::ComponentAttacher() {

}

ComponentAttacher::~ComponentAttacher() {

}

void Valkyrie::ComponentAttacher::attachComponent(const Scene::ObjectPtr& object_ptr, const ValkyrieComponent::ComponentPtr& component_ptr) {
	if (object_ptr != nullptr && component_ptr != nullptr) {
		component_ptr->objectWeakPtr = object_ptr;
		auto&& component_name = component_ptr->getName();
		object_ptr->m_component_ptrs.push_back(component_ptr);
		object_ptr->m_component_index[component_name] = object_ptr->m_component_ptrs.size() - 1;
	}
}
