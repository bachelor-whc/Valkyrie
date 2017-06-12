#include "valkyrie/factory/object.h"
#include "common.h"
using namespace Valkyrie;

ObjectFactory* ObjectFactory::gp_object_factory = nullptr;

int ObjectFactory::initialize() {
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

ObjectFactory::ObjectFactory() {

}

ObjectFactory::~ObjectFactory() {

}
