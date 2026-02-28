#include "resource.h"
#include "lab_m1/tema2/my_utils/common_meshes.h"

void Resource::Render(Shader* shader, glm::mat4& modelMatrix,
	const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	if (!mesh) return;

	if (type == PLANKS || type == DOORS) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.0f, 0.2f));
	}

	if (type == WOOD) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.5f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
	}

	shader->Use();
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	mesh->Render();
}

namespace ResourceFactory {
	Resource* CreateWoodResource() {
		return new Resource(commonMeshes::getMesh("wood"), WOOD);
	}
	Resource* CreateIronResource() {
		return new Resource(commonMeshes::getMesh("iron"), IRON);
	}
	Resource* CreatePlanksResource() {
		return new Resource(commonMeshes::getMesh("planks"), PLANKS,
			{ WOOD }, { 2 });
	}
	Resource* CreateNailsResource() {
		return new Resource(commonMeshes::getMesh("nails"), NAILS,
			{ IRON }, { 1 });
	}
	Resource* CreateSteelResource() {
		return new Resource(commonMeshes::getMesh("steel"), STEEL,
			{ IRON }, { 2 });
	}
	Resource* CreateDoorsResource() {
		return new Resource(commonMeshes::getMesh("doors"), DOORS,
			{ PLANKS, NAILS }, { 2, 4 });
	}

	Resource* CreateResourceByType(ResourceType type) {
		switch (type) {
		case WOOD:
			return CreateWoodResource();
		case IRON:
			return CreateIronResource();
		case PLANKS:
			return CreatePlanksResource();
		case NAILS:
			return CreateNailsResource();
		case STEEL:
			return CreateSteelResource();
		case DOORS:
			return CreateDoorsResource();
		default:
			return nullptr;
		}
	}
};

std::string ResourceTypeToString(ResourceType type) {
	switch (type) {
	case WOOD:
		return "wood";
	case IRON:
		return "iron";
	case PLANKS:
		return "planks";
	case NAILS:
		return "nails";
	case STEEL:
		return "steel";
	case DOORS:
		return "doors";
	default:
		return "unknown";
	}
}
