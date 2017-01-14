#ifndef _VALKYRIE_COMPONENT_ATTACHER_H
#define _VALKYRIE_COMPONENT_ATTACHER_H
#include <memory>

namespace ValkyrieComponent {
	class Component;
	using ComponentPtr = std::shared_ptr<Component>;
}

namespace Valkyrie {
	namespace Scene {
		class Object;
		using ObjectPtr = std::shared_ptr<Object>;
	}

	class ComponentAttacher {
	public:
		ComponentAttacher();
		virtual ~ComponentAttacher();

		void attachComponent(const Scene::ObjectPtr& object_ptr, const ValkyrieComponent::ComponentPtr& component_ptr);
	private:

	};
}

#endif