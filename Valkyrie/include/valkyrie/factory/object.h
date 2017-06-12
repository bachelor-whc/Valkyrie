#ifndef _VALKYRIE_OBJECT_FACTORY_H
#define _VALKYRIE_OBJECT_FACTORY_H
#include "valkyrie/base_unit.h"
#include "valkyrie/scene/object.h"

namespace ValkyrieFactory {
	class ObjectFactory {
	public:
		static int initialize();
		static void close();
		static ObjectFactory& instance() { return *gp_object_factory; }

		virtual ~ObjectFactory();
		
		Valkyrie::Scene::ObjectPtr createObject();

	private:
		static ObjectFactory* gp_object_factory;
		ObjectFactory();
	};
}

#endif