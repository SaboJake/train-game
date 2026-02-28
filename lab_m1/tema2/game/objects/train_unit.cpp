#include "train_unit.h"

TrainUnit::TrainUnit(const std::string& name, const std::vector<Mesh*>& meshes)
	: name(name), position(0, 0, 0), rotation(0, 0, 0), scale(1.0f), hasCargo(false), acceptingCargo(false)
{
}

TrainUnit::~TrainUnit()
{
}

void TrainUnit::Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
}

void TrainUnit::RenderPart(Mesh* mesh, Shader* shader, const glm::mat4& parentMatrix,
	glm::vec3 offset, glm::vec3 localScale, glm::vec3 axisRot, float angleRot) {
	glm::mat4 modelMatrix = parentMatrix;
	modelMatrix = glm::translate(modelMatrix, offset);
	if (angleRot != 0.0f) {
		modelMatrix = glm::rotate(modelMatrix, glm::radians(angleRot), axisRot);
	}
	modelMatrix = glm::scale(modelMatrix, localScale);

	shader->Use();
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	mesh->Render();
}
