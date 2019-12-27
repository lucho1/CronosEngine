#ifndef _ASSIMPIMPORTER_H_
#define _ASSIMPIMPORTER_H_

#include "GameObject/GameObject.h"
#include "Renderer/Textures.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceMaterial.h"

#include <Assimp/include/cimport.h>
#include <Assimp/include/cfileio.h>
#include <Assimp/include/Importer.hpp>
#include <Assimp/include/scene.h>
#include <Assimp/include/postprocess.h>

namespace Cronos {

	class AssimpCronosImporter
	{
		friend class Scene;
		friend class Filesystem;
		friend class AssetItems;

	private:

		AssimpCronosImporter();

		GameObject* LoadModel(const std::string& filepath);
		void ProcessAssimpNode(aiNode* as_node, const aiScene* as_scene, GameObject* motherGameObj);
		void ProcessCronosMesh(aiMesh* as_mesh, const aiScene* as_scene, GameObject* motherGameObj, aiNode* as_node);

		Texture* LoadTextures(aiMaterial* material, TextureType TexType, const std::string& GOPath);
		void LoadSceneMaterials(const aiScene* as_scene, const std::string& path);

	private:

		uint MeshNum = 0;
		std::vector<Material*> m_SceneCronosMaterials; //Materials of each scene
	};

}
#endif