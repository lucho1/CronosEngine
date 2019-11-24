#ifndef _MESHCOMPONENT_H_
#define _MESHCOMPONENT_H_

#include "glm/glm.hpp"

#include "Component.h"
#include "Renderer/VertexArray.h"
#include "MaterialComponent.h"
#include "Resources/ResourceMesh.h"

namespace Cronos {

	struct CronosVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		CronosVertex(){};
		CronosVertex(std::vector<float>a) { Position.x = a[0], Position.y = a[1], Position.z = a[2], Normal.x = a[3], Normal.y = a[4], Normal.z = a[5], TexCoords.x = a[6], TexCoords.y = a[7]; }
		
	};

	class MeshComponent : public Component
	{
	public:

		MeshComponent(GameObject* attachedGO);
		~MeshComponent();

		virtual void Update(float dt) override;
		void Draw(MaterialComponent* material, bool bindMaterial);

		void SetupMesh(std::vector<CronosVertex>vertices, std::vector<uint>indices);
		void RecalculateNormals() { m_NormalsCalculated = false; }
		bool &setDebugDraw() { RecalculateNormals(); return m_DebugDraw; }

		//Getters
		const inline std::vector<CronosVertex> GetVertexVector() const { return m_VertexVector; }
		const inline std::vector<uint> GetIndexVector() const { return m_IndicesVector; }
		inline VertexArray* GetVAO() const { return m_MeshVAO; }

		static ComponentType GetType() { return ComponentType::MESH; }
		std::vector<CronosVertex> m_VertexVector;

		ResourceMesh* r_mesh;

	private:

		void CalculateNormals(std::vector<glm::vec3>& normals, std::vector<glm::vec3>& positions);

		void DrawVerticesNormals();
		void DrawPlanesNormals();

	private:

		
		std::vector<uint> m_IndicesVector;

		VertexArray* m_MeshVAO = nullptr;
		VertexBuffer* m_MeshVBO = nullptr;
		IndexBuffer* m_MeshIBO = nullptr;



		bool m_DebugDraw = false;
		bool m_NormalsCalculated = false;
	};
}

#endif