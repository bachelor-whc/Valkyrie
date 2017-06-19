#include "valkyrie/scene/object.h"
#include "valkyrie/component/component.h"
using namespace Valkyrie::Scene;

Object::Object() :
	transform() {

}

Object::~Object() {

}

void Object::start() {

}

void Object::update() {
	transform.update();
	for (auto& component_ptr : m_component_ptrs) {
		if(component_ptr->isEnabled())
			component_ptr->update();
	}
}

ValkyrieComponent::ComponentPtr Object::getComponentPtr(const std::string& component_name) {
	if (m_component_index.count(component_name) > 0)
		return m_component_ptrs[m_component_index[component_name]];
	return nullptr;
}
