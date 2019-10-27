#ifndef _MESHCOMPONENT_H_
#define _MESHCOMPONENT_H_

#include "glm/glm.hpp"
#include "Component.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Textures.h"
#include "Renderer/Shaders.h"

namespace Cronos {

	struct CronosVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	class MeshComponent : public Component
	{
	public:

		MeshComponent(GameObject* GObjAttached);
		~MeshComponent();

		virtual void Update(float dt) override;
		void Draw(Shader* shader, bool bindShader);

		void SetupMesh(std::vector<CronosVertex>vertices, std::vector<uint>indices, std::vector<Texture*> textures);
		void SetTextures(std::vector<Texture*> newTexture, TextureType textureType);
		void RecalculateNormals() { m_NormalsCalculated = false; }
		
		//Getters
		const std::vector<Texture*>& GetTexturesVector() const { return m_TexturesVector; }
		const std::vector<CronosVertex> GetVertexVector() const { return m_VertexVector; }
		const std::vector<uint> GetIndexVector() const { return m_IndicesVector; }
		static ComponentType GetType() { return ComponentType::MESH; };


	private:

		void CalculateNormals(std::vector<glm::vec3>& normals, std::vector<glm::vec3>& positions);

		void DrawVerticesNormals();
		void DrawPlanesNormals();
		void DrawCentralAxis();

	private:

		std::vector<Texture*> m_TexturesVector;
		std::vector<CronosVertex> m_VertexVector;
		std::vector<uint> m_IndicesVector;

		VertexArray* m_MeshVAO = nullptr;
		VertexBuffer* m_MeshVBO = nullptr;
		IndexBuffer* m_MeshIBO = nullptr;

		bool m_DebugDraw = false;
		bool m_NormalsCalculated = false;
	};
}

#endif