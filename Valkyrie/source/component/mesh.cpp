#include "valkyrie/component/mesh.h"
using namespace ValkyrieComponent;

Mesh::Mesh(const MeshSupportPtr& mesh_ptr) : 
	m_vulkan_buffer(),
	m_mesh_support_ptr(mesh_ptr) {

}