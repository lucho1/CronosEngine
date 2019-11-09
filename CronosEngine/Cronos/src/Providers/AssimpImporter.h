#ifndef _ASSIMPIMPORTER_H_
#define _ASSIMPIMPORTER_H_

#include "GameObject/GameObject.h"
#include "Renderer/Textures.h"

#include <Assimp/include/cimport.h>
#include <Assimp/include/cfileio.h>
#include <Assimp/include/Importer.hpp>
#include <Assimp/include/scene.h>
#include <Assimp/include/postprocess.h>

namespace Cronos {

	class AssimpCronosImporter
	{
		friend class Scene;
	private:

		AssimpCronosImporter();
		GameObject* LoadModel(const std::string& filepath);

		void ProcessAssimpNode(aiNode* as_node, const aiScene* as_scene, GameObject* motherGameObj);
		void ProcessCronosMesh(aiMesh* as_mesh, const aiScene* as_scene, GameObject* motherGameObj);

		std::vector<Texture*> LoadTextures(aiMaterial *material, aiTextureType Texturetype, TextureType texType, GameObject* motherGameObj);
		void SetTexturesVector(aiMesh* as_mesh, const aiScene* as_scene, GameObject* GObj, std::vector<Texture*>& TexVec);

		std::vector<Texture*> m_TexturesLoaded;

		glm::vec3 m_AABB_MinVec;
		glm::vec3 m_AABB_MaxVec;
		uint MeshNum = 0;
		
	};

}
#endif