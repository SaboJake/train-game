#include "train_car.h"
#include "lab_m1/tema2/my_utils/common_meshes.h"
#include <glm/gtc/matrix_transform.hpp>

TrainCar::TrainCar(const std::string& name, Mesh* meshBody, Mesh* meshCabin, Mesh* meshWheel)
	: TrainUnit(name, {}),
	  meshBody(meshBody), meshCabin(meshCabin), meshWheel(meshWheel)
{
	hasCargo = false;
	acceptingCargo = true;
}

TrainCar::TrainCar(const std::string& name, const std::vector<Mesh*>& meshes)
	: TrainUnit(name, meshes)
{
	hasCargo = false;
	acceptingCargo = true;
	if (meshes.size() >= 3) {
		meshBody = meshes[0];
		meshCabin = meshes[1];
		meshWheel = meshes[2];
	} else {
		// Handle error: not enough meshes provided
		meshBody = nullptr;
		meshCabin = nullptr;
		meshWheel = nullptr;
	}
}

TrainCar::~TrainCar()
{
}

void TrainCar::RenderPart(Mesh* mesh, Shader* shader, const glm::mat4& parentMatrix,
	glm::vec3 offset, glm::vec3 localScale, glm::vec3 axisRot, float angleRot) {
	__super::RenderPart(mesh, shader, parentMatrix, offset, localScale, axisRot, angleRot);
}

void TrainCar::Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glm::mat4 trainCarMatrix = glm::mat4(1);
	trainCarMatrix = glm::translate(trainCarMatrix, position);
	trainCarMatrix = glm::rotate(trainCarMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	trainCarMatrix = glm::rotate(trainCarMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	trainCarMatrix = glm::rotate(trainCarMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	trainCarMatrix = glm::scale(trainCarMatrix, glm::vec3(scale));
	
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	RenderPart(meshBody, shader, trainCarMatrix,
		glm::vec3(0, 0, 0),
		glm::vec3(4.0f, 0.2f, 1.5f),
		glm::vec3(1, 0, 0), 0.0f);
	RenderPart(meshCabin, shader, trainCarMatrix,
		glm::vec3(0, 0.6f, 0),
		glm::vec3(4.0f, 1.0f, 1.5f),
		glm::vec3(1, 0, 0), 0.0f);

	float wx = 1.5f; float wz = 0.8f; float wy = -0.2f;
	RenderPart(meshWheel, shader, trainCarMatrix, glm::vec3(wx, wy, wz), glm::vec3(0.8f, 0.2f, 0.8f), glm::vec3(1, 0, 0), 90);
	RenderPart(meshWheel, shader, trainCarMatrix, glm::vec3(wx, wy, -wz), glm::vec3(0.8f, 0.2f, 0.8f), glm::vec3(1, 0, 0), 90);
	RenderPart(meshWheel, shader, trainCarMatrix, glm::vec3(-wx, wy, wz), glm::vec3(0.8f, 0.2f, 0.8f), glm::vec3(1, 0, 0), 90);
	RenderPart(meshWheel, shader, trainCarMatrix, glm::vec3(-wx, wy, -wz), glm::vec3(0.8f, 0.2f, 0.8f), glm::vec3(1, 0, 0), 90);
}

std::vector<Mesh*> CreateTrainCarMeshes() {
	std::vector<Mesh*> trainMeshes;
	trainMeshes.push_back(commonMeshes::getMesh("train_body"));
	trainMeshes.push_back(commonMeshes::getMesh("train_cabin"));
	trainMeshes.push_back(commonMeshes::getMesh("train_wheel"));

	return trainMeshes;
}
