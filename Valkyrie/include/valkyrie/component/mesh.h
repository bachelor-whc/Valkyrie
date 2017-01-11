#ifndef _VALKYRIECOMPONENT_MESH_H
#define _VALKYRIECOMPONENT_MESH_H
#include "valkyrie/component/component.h"
#include "valkyrie/vulkan/memory_buffer.h"

namespace ValkyrieComponent {
	class MeshSupport {
	public:
		MeshSupport() {};
		virtual ~MeshSupport() {};

	private:
		
	};

	using MeshSupportPtr = std::shared_ptr<MeshSupport>;

	class Mesh : public Component {
	public:
		Mesh() = delete;
		Mesh(const MeshSupportPtr& mesh_ptr);
		virtual ~Mesh() {};

	private:
		Vulkan::MemoryBuffer m_vulkan_buffer;
		MeshSupportPtr m_mesh_support_ptr;
	};
}

#endif