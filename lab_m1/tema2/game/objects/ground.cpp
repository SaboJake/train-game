#include "ground.h"

Mesh* Ground::GetMesh(int type) {
	switch (type) {
	case 0:
		return object3D::CreateQuad("ground_grass", glm::vec3(0), 1.0f, 1.0f, glm::vec3(0, 0.75, 0), true);
	case 1:
		return object3D::CreateQuad("ground_water", glm::vec3(0), 1.0f, 1.0f, glm::vec3(0, 0, 1), true);
	case 2:
		return object3D::CreateQuad("ground_hill", glm::vec3(0), 1.0f, 1.0f, glm::vec3(0.5f, 0.35f, 0.05f), true);
	default:
		return nullptr;
	}
}

void Ground::Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, pos);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(tileSize, 1.0f, tileSize));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	if (mesh) {
		mesh->Render();
	}
}
