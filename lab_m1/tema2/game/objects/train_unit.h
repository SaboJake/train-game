#pragma once

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "lab_m1/tema2/my_utils/object3D.h"

class TrainUnit {
public:
	TrainUnit(const std::string& name, const std::vector<Mesh*>& meshes);
	~TrainUnit();

	virtual void Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	glm::vec3 GetPosition() { return position; }
	glm::vec3 GetRotation() { return rotation; }
	float GetScale() { return scale; }

	void SetPosition(const glm::vec3& position) { this->position = position; }
	void SetRotation(const glm::vec3& rotation) { this->rotation = rotation; }
	void SetScale(float scale) { this->scale = scale; }

	bool hasCargo;
	bool acceptingCargo;

protected:
	std::string name;

	glm::vec3 position;
	glm::vec3 rotation;
	float scale;
protected:
	void RenderPart(Mesh* mesh, Shader* shader, const glm::mat4& parentMatrix,
		glm::vec3 offset, glm::vec3 scale, glm::vec3 axisRot, float angleRot);
};
