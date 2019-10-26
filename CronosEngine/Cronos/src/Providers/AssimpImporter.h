#ifndef _ASSIMPIMPORTER_H_
#define _ASSIMPIMPORTER_H_

#include "GameObject/GameObject.h"
#include "GameObject/Components/MeshComponent.h"
#include "Renderer/Textures.h"

#include <Assimp/include/cimport.h>
#include <Assimp/include/cfileio.h>
#include <Assimp/include/Importer.hpp>
#include <Assimp/include/scene.h>
#include <Assimp/include/postprocess.h>

namespace Cronos {

	class AssimpCronosImporter
	{
	public:

		AssimpCronosImporter();
		GameObject* LoadModel(const std::string& filepath);

	private:

		void ProcessAssimpNode(aiNode* as_node, const aiScene* as_scene, GameObject* motherGameObj);
		void ProcessCronosMesh(aiMesh* as_mesh, const aiScene* as_scene, GameObject* motherGameObj);

		std::vector<Texture*> LoadTextures(aiMaterial *material, aiTextureType Texturetype, TextureType texType, GameObject* motherGameObj);
	};

}
#endif