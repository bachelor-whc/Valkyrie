#ifndef _VALKYRIECOMPONENT_CAMERA_H
#define _VALKYRIECOMPONENT_CAMERA_H
#include "valkyrie/component/component.h"

namespace ValkyrieComponent {
	class MeshRenderer : public Component {
	public:
		MeshRenderer() = delete;
		MeshRenderer(const Valkyrie::MeshPtr& mesh_ptr);
		virtual ~MeshRenderer() {};

		virtual void start() {};
		virtual void update() {};
		void recordDrawCommand(Vulkan::CommandBuffer& command);

	private:
		Valkyrie::MeshPtr m_mesh_ptr;
	};
}

#endif