#ifndef _VALKYRIEFACTORY_MESH_h
#define _VALKYRIEFACTORY_MESH_h
#include "valkyrie/asset/mesh_asset.h"
#include "valkyrie/asset/lavy_asset.h"

namespace ValkyrieFactory {
	class MeshFactory {
	public:
		static int initialize();
		static void close();
		static bool initialized();
		static MeshFactory& instance() { return *gp_mesh_factory; }

		virtual ~MeshFactory();

		Valkyrie::MeshPtr createMesh(const Valkyrie::LavyAssetPtr lavy_ptr);

	private:
		static MeshFactory* gp_mesh_factory;
		MeshFactory();
	};
}

#endif