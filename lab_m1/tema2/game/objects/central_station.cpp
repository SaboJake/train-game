#include "central_station.h"
#include "lab_m1/tema2/my_utils/common_meshes.h"

#include <iostream>

CentralStation::CentralStation(Mesh* mesh,
	glm::vec3 position,
	const std::vector<Platform>& platforms)
	: Station(mesh, position, platforms, {}, {}), ordersFulfilled(0)
{
	requestManager = new RequestManager();
	requestManager->GetNewRequest(this->acceptedResources, this->requestedResources);
}

void CentralStation::RenderStation(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	float stationScale = 4.0f;
	float baseOffsets[2] = { -7.0f, 7.0f };


	for (float offset : baseOffsets) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, position - glm::vec3(0, position.y - stationScale / 2, 0));
		modelMatrix = glm::rotate(modelMatrix, -glm::radians(rotationAngle), glm::vec3(0, 1, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(offset, 0.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(stationScale));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f, 1.0f, 1.5f));
		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		commonMeshes::getMesh("station1_base")->Render();
	}

	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, position - glm::vec3(0, position.y - stationScale / 2, 0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, stationScale / 2.0f + 0.4f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, -glm::radians(rotationAngle), glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(stationScale));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(7.0f, 0.2f, 2.0f));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	commonMeshes::getMesh("station2_roof")->Render();

	float pillarOffsetsX[4] = { -12.0f, 12.0f, -2.0f, 2.0f };
	float pillarOffsetsZ[2] = { -2.0f, 2.0f };

	for (float offsetX : pillarOffsetsX) {
		for (float offsetZ : pillarOffsetsZ) {
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, position - glm::vec3(0, position.y - stationScale / 2, 0));
			modelMatrix = glm::rotate(modelMatrix, -glm::radians(rotationAngle), glm::vec3(0, 1, 0));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(offsetX, 0.0f, offsetZ));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(stationScale));
			glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			commonMeshes::getMesh("station2_pillar")->Render();
		}
	}
}

void CentralStation::Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (!mesh) return;

	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	RenderStation(shader, viewMatrix, projectionMatrix);

	glm::vec3 upOffset = glm::vec3(0.0f, 3.0f, 0.0f);

	// Render request
	for (int i = 0; i < requestedResources.size(); i++) {
		Resource* resource = acceptedResources[i];
		int amount = requestedResources[i];

		for (int j = 0; j < amount; j++) {
			glm::mat4 resourceModelMatrix = glm::mat4(1);
			resourceModelMatrix = glm::translate(resourceModelMatrix, position + glm::vec3(-2 - i * 4.0f, 3.0f + j * 1.2f, 0.0f));
			resourceModelMatrix = glm::translate(resourceModelMatrix, upOffset);
			resourceModelMatrix = glm::scale(resourceModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
			glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(resourceModelMatrix));
			resource->Render(shader, resourceModelMatrix, viewMatrix, projectionMatrix);
		}
	}

	// Render accepted resources
	for (int i = 0; i < acceptedResources.size(); i++) {
		Resource* resource = acceptedResources[i];
		int amount = availableResources[static_cast<int>(resource->GetType())];

		for (int j = 0; j < amount; j++) {
			glm::mat4 resourceModelMatrix = glm::mat4(1);
			resourceModelMatrix = glm::translate(resourceModelMatrix, position + glm::vec3(2 + i * 4.0f, 3.0f + j * 1.2f, 0.0f));
			resourceModelMatrix = glm::translate(resourceModelMatrix, upOffset);
			resourceModelMatrix = glm::scale(resourceModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
			glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(resourceModelMatrix));
			resource->Render(shader, resourceModelMatrix, viewMatrix, projectionMatrix);
		}
	}
}

void CentralStation::GetNextRequest() {
	for (Resource* res : acceptedResources) {
		delete res;
	}
	acceptedResources.clear();
	requestedResources.clear();
	requestManager->GetNewRequest(acceptedResources, requestedResources);
}

void CentralStation::Unload() {
	Station::Unload();
	
	// Check if requested resources are fulfilled
	int resourcesFulfilled = 0;
	for (int i = 0; i < requestedResources.size(); i++) {
		ResourceType rType = acceptedResources[i]->GetType();
		if (availableResources[static_cast<int>(rType)] >= requestedResources[i]) {
			requestedResources[i] = 0;
			availableResources[static_cast<int>(rType)] = 0;
			resourcesFulfilled++;
		}
	}
	std::cout << "Central Station: " << resourcesFulfilled << " out of " << requestedResources.size() << " resource types fulfilled." << std::endl;
	if (resourcesFulfilled == requestedResources.size()) {
		ordersFulfilled++;
		GetNextRequest();
	}
}
