#ifndef _VALKYRIECOMPONENT_MESH_H
#define _VALKYRIECOMPONENT_MESH_H
#include "valkyrie/component/component.h"
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/memory_buffer.h"
#include "valkyrie/asset/mesh_asset.h"

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