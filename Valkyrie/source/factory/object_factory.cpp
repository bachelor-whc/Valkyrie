#include "valkyrie/common.h"
#include "valkyrie/factory/object.h"
#include "valkyrie/scene/object_manager.h"
#include "valkyrie/scene/object.h"
#include "valkyrie/scene/camera.h"
#include "valkyrie/component/component_attacher.h"
#include "valkyrie/scene/light_position.h"
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

ObjectFactory::ObjectFactory() : m_component_attacher() {

}

template<typename T>
std::shared_ptr<T> ObjectFactory::processObjectCreationRoutine() {
	auto& object_manager = ObjectManager::instance();
	std::shared_ptr<T> ptr = MAKE_SHARED(T)();
	std::shared_ptr<Scene::Object> dptr = ptr;
	acquireID(dptr, object_manager);
	registerObject(dptr, object_manager);
	return ptr;
}

void ObjectFactory::acquireID(std::shared_ptr<Valkyrie::Scene::Object>& ptr, ObjectManager& manager) {
	ptr->m_ID = manager.acquireNextID();
}

void ObjectFactory::registerObject(std::shared_ptr<Valkyrie::Scene::Object>& ptr, ObjectManager& manager) {
	manager.registerObject(ptr);
}

ObjectFactory::~ObjectFactory() {

}

Scene::ObjectPtr ObjectFactory::createObject() {
	return processObjectCreationRoutine<Scene::Object>();
}

Scene::CameraPtr ObjectFactory::createCamera(float fov, float ratio, float _near, float _far) {
	auto& ptr = processObjectCreationRoutine<Scene::Camera>();
	m_component_attacher.attachComponent(ptr, MAKE_SHARED(ValkyrieComponent::Camera)(fov, ratio, _near, _far));
	return ptr;
}

Scene::LightPtr ObjectFactory::createLight(Valkyrie::Scene::Light::Type type) {
    Scene::LightPtr ptr = nullptr;
    switch (type)
    {
    case Valkyrie::Scene::Light::POSITION:
        ptr = processObjectCreationRoutine<Scene::PositionLight>();
        break;
    case Valkyrie::Scene::Light::DIRECTIONAL:
        assert(false);
        break;
    case Valkyrie::Scene::Light::SPOTLIGHT:
        assert(false);
        break;
    default:
        assert(false);
        break;
    }
    return ptr;
}
