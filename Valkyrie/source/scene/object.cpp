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
		component_ptr->update();
	}
}