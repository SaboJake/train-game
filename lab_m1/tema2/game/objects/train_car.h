#pragma once

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "lab_m1/tema2/my_utils/object3D.h"
#include "lab_m1/tema2/game/objects/train_unit.h"
#include "lab_m1/tema2/game/resource.h"
class TrainCar: public TrainUnit {
public:
	TrainCar(const std::string& name, Mesh* meshBody, Mesh* meshCabin, Mesh* meshWheel);
	TrainCar(const std::string& name, const std::vector<Mesh*>& meshes);
	~TrainCar();

	void Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	
	glm::vec3 GetPosition() { return position; }
	glm::vec3 GetRotation() { return rotation; }
	float GetScale() { return scale; }

	void SetPosition(const glm::vec3& position) { this->position = position; }
	void SetRotation(const glm::vec3& rotation) { this->rotation = rotation; }
	void SetScale(float scale) { this->scale = scale; }
private:
	Mesh* meshBody;
	Mesh* meshCabin;
	Mesh* meshWheel;

	void RenderPart(Mesh* mesh, Shader* shader, const glm::mat4& parentMatrix,
		glm::vec3 offset, glm::vec3 scale, glm::vec3 axisRot, float angleRot);
};

std::vector<Mesh*> CreateTrainCarMeshes();
