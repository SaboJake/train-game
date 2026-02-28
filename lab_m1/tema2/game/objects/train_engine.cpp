#include "train_engine.h"
#include "lab_m1/tema2/my_utils/common_meshes.h"
#include <glm/gtc/matrix_transform.hpp>

TrainEngine::TrainEngine(const std::string& name, Mesh* meshBody, Mesh* meshCabin, Mesh* meshWheel, Mesh* meshBoiler)
	: TrainUnit(name, {}),
      meshBody(meshBody), meshCabin(meshCabin), meshWheel(meshWheel), meshBoiler(meshBoiler)
{
	hasCargo = false;
	acceptingCargo = false;
}

TrainEngine::TrainEngine(const std::string& name, const std::vector<Mesh*>& meshes)
	: TrainUnit(name, meshes)
{
	hasCargo = false;
	acceptingCargo = false;
    if (meshes.size() >= 4) {
        meshBody = meshes[0];
        meshBoiler = meshes[1];
        meshCabin = meshes[2];
        meshWheel = meshes[3];
    }
    else {
        meshBody = nullptr;
        meshBoiler = nullptr;
        meshCabin = nullptr;
        meshWheel = nullptr;
    }
}

TrainEngine::~TrainEngine()
{
}

void TrainEngine::RenderPart(Mesh* mesh, Shader* shader, const glm::mat4& parentMatrix,
    glm::vec3 offset, glm::vec3 localScale, glm::vec3 axisRot, float angleRot) {
	__super::RenderPart(mesh, shader, parentMatrix, offset, localScale, axisRot, angleRot);
}

void TrainEngine::Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glm::mat4 trainMatrix = glm::mat4(1);
    trainMatrix = glm::translate(trainMatrix, position);
    trainMatrix = glm::rotate(trainMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    trainMatrix = glm::rotate(trainMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    trainMatrix = glm::rotate(trainMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    trainMatrix = glm::scale(trainMatrix, glm::vec3(scale));

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    RenderPart(meshBody, shader, trainMatrix,
        glm::vec3(0, 0, 0),
        glm::vec3(5.0f, 0.2f, 1.5f),
        glm::vec3(1, 0, 0), 0);

    RenderPart(meshCabin, shader, trainMatrix,
        glm::vec3(-1.5f, 0.8f, 0),
        glm::vec3(2.0f, 1.5f, 1.4f),
        glm::vec3(1, 0, 0), 0);

    RenderPart(meshBoiler, shader, trainMatrix,
        glm::vec3(1.0f, 0.6f, 0),
        glm::vec3(1.0f, 3.0f, 1.0f),
        glm::vec3(0, 0, 1), -90.0f);


    float wz = 0.8f;
    float wy = -0.3f;
    glm::vec3 wheelScale(0.8f, 0.2f, 0.8f);

    float wxBack1 = -2.0f;
    float wxBack2 = -1.0f;
    RenderPart(meshWheel, shader, trainMatrix, glm::vec3(wxBack1, wy, wz), wheelScale, glm::vec3(1, 0, 0), 90);
    RenderPart(meshWheel, shader, trainMatrix, glm::vec3(wxBack1, wy, -wz), wheelScale, glm::vec3(1, 0, 0), 90);
    RenderPart(meshWheel, shader, trainMatrix, glm::vec3(wxBack2, wy, wz), wheelScale, glm::vec3(1, 0, 0), 90);
    RenderPart(meshWheel, shader, trainMatrix, glm::vec3(wxBack2, wy, -wz), wheelScale, glm::vec3(1, 0, 0), 90);

    float wxFront1 = 0.5f;
    float wxFront2 = 1.4f;
    float wxFront3 = 2.3f;
    RenderPart(meshWheel, shader, trainMatrix, glm::vec3(wxFront1, wy, wz), wheelScale, glm::vec3(1, 0, 0), 90);
    RenderPart(meshWheel, shader, trainMatrix, glm::vec3(wxFront1, wy, -wz), wheelScale, glm::vec3(1, 0, 0), 90);
    RenderPart(meshWheel, shader, trainMatrix, glm::vec3(wxFront2, wy, wz), wheelScale, glm::vec3(1, 0, 0), 90);
    RenderPart(meshWheel, shader, trainMatrix, glm::vec3(wxFront2, wy, -wz), wheelScale, glm::vec3(1, 0, 0), 90);
    RenderPart(meshWheel, shader, trainMatrix, glm::vec3(wxFront3, wy, wz), wheelScale, glm::vec3(1, 0, 0), 90);
    RenderPart(meshWheel, shader, trainMatrix, glm::vec3(wxFront3, wy, -wz), wheelScale, glm::vec3(1, 0, 0), 90);
}

std::vector<Mesh*> CreateTrainMeshes() {
    std::vector<Mesh*> trainMeshes;

    trainMeshes.push_back(commonMeshes::getMesh("train_body"));
	trainMeshes.push_back(commonMeshes::getMesh("train_boiler"));
	trainMeshes.push_back(commonMeshes::getMesh("train_cabin"));
	trainMeshes.push_back(commonMeshes::getMesh("train_wheel"));

    return trainMeshes;
}
