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
	};

	class MeshComponent : public Component
	{
	public:

		//Component Methods
		MeshComponent(GameObject* attachedGO);
		~MeshComponent();

		virtual void Update(float dt) override;
		void Draw(MaterialComponent* material, bool bindMaterial);

		void SetupMesh(std::vector<CronosVertex>vertices, std::vector<uint>indices);
		bool &setDebugDraw() { return m_DebugDraw; }
		bool &SetDrawAxis() { return m_DrawAxis; }

	public:

		//Getters
		const inline std::vector<CronosVertex> GetVertexVector()	const { return m_VertexVector; }
		const inline std::vector<uint> GetIndexVector()				const { return m_IndicesVector; }
		inline VertexArray* GetVAO()								const { return m_MeshVAO; }

		static ComponentType GetType()									  { return ComponentType::MESH; }		
		
	private:

		void DrawCentralAxis();
		void DrawVerticesNormals();
		void DrawPlanesNormals();

	public:

		std::vector<CronosVertex> m_VertexVector;
		ResourceMesh* r_mesh;

	private:
		
		std::vector<uint> m_IndicesVector;

		VertexArray* m_MeshVAO = nullptr;
		VertexBuffer* m_MeshVBO = nullptr;
		IndexBuffer* m_MeshIBO = nullptr;

		bool m_DebugDraw = false;
		bool m_DrawAxis = false;
	};
}

#endif