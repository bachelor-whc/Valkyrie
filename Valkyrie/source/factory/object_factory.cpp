#include "valkyrie/factory/object.h"
#include "valkyrie/scene/object_manager.h"
#include "common.h"
using namespace Valkyrie;
using namespace ValkyrieFactory;

ObjectFactory* ObjectFactory::gp_object_factory = nullptr;

int ObjectFactory::initialize() {
	assert(ObjectManager::initialized());
	gp_object_factory = NEW_NT ObjectFactory;
	if (gp_object_factory == nullptr)
		return 1;
	return 0;
}

void ObjectFactory::close() {
	if (gp_object_factory != nullptr) {
		delete gp_object_factory;
		gp_object_factory = nullptr;
	}
}

bool ObjectFactory::initialized() {
	return gp_object_factory != nullptr;
}

ObjectFactory::ObjectFactory() {

}

ObjectFactory::~ObjectFactory() {

}

Scene::ObjectPtr ObjectFactory::createObject() {
	auto& object_manager = Valkyrie::ObjectManager::instance();
	Scene::ObjectPtr object_ptr = MAKE_SHARED(Scene::Object)();
	object_ptr->m_ID = object_manager.acquireNextID();
	object_manager.registerObject(object_ptr);
	return object_ptr;
}
