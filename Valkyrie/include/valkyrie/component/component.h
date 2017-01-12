#ifndef _VALKYRIECOMPONENT_COMPONENT_H
#define _VALKYRIECOMPONENT_COMPONENT_H
#include <memory>
#include "valkyrie/base_unit.h"

namespace Valkyrie {
	namespace Scene {
		class Object;
	}
}

namespace ValkyrieComponent {
	class Component : public Valkyrie::Base {
	public:
		Component() {};
		virtual ~Component() {};

		std::weak_ptr<Valkyrie::Scene::Object> objectWeakPtr;
	};

	using ComponentPtr = std::shared_ptr<Component>;
}

#endif