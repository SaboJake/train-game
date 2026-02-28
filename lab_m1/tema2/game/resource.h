#pragma once

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"

#include <vector>

enum ResourceType {
	WOOD,
	IRON,
	PLANKS,
	NAILS,
	STEEL,
	DOORS
};

class Resource {
public:
	Resource(Mesh* mesh, ResourceType type,
		const std::vector<ResourceType>& recepie,
		const std::vector<int>& recepieAmounts)
		: mesh(mesh), type(type), recepie(recepie), recepieAmounts(recepieAmounts) {
	}

	Resource(Mesh* mesh, ResourceType type)
		: mesh(mesh), type(type), recepie({}), recepieAmounts({}) {
	}

	ResourceType GetType() const { return type; }
	bool HasRecepie() const { return !recepie.empty(); }
	std::vector<ResourceType> GetRecepie() const { return recepie; }
	std::vector<int> GetRecepieAmounts() const { return recepieAmounts; }

	void SetResourceType(ResourceType newType) { type = newType; }

	void Render(Shader* shader, glm::mat4& modelMatrix,
		const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);


private:
	Mesh* mesh;
	ResourceType type;
	std::vector<ResourceType> recepie;
	std::vector<int> recepieAmounts;
};

namespace ResourceFactory {
	Resource* CreateWoodResource();
	Resource* CreateIronResource();
	Resource* CreatePlanksResource();
	Resource* CreateNailsResource();
	Resource* CreateSteelResource();
	Resource* CreateDoorsResource();
	Resource* CreateResourceByType(ResourceType type);
}

std::string ResourceTypeToString(ResourceType type);
