#include "station.h"
#include "lab_m1/tema2/my_utils/common_meshes.h"

#include <iostream>


Station::Station(Mesh* mesh, glm::vec3 position,
	const std::vector<Platform>& platforms,
	const std::vector<Resource*>& productionResources,
	const std::vector<Resource*>& acceptedResources)
	: mesh(mesh), position(position), platforms(platforms), productionResources(productionResources), acceptedResources(acceptedResources)
{
	availableResources.resize(10, 0);
	rotationAngle = 0.0f;

	design = 1;
}



void Station::Update(float deltaTime, const std::vector<Train*>& trains) {
	if (productionResources.size() > 0) {
		currentProductionTimer += deltaTime;
		if (currentProductionTimer >= productionTime) {
			Produce();
			currentProductionTimer = 0.0f;
		}
	}

	for (auto& platform : platforms) {
		if (platform.processedTrain) {
			bool onRail = false;
			if (!platform.processedTrain->GetCarStates().empty()) {
				if (platform.processedTrain->GetCarStates()[0].currentRail == platform.GetRail()) {
					onRail = true;
				}
			}
			if (!onRail) {
				platform.processedTrain = nullptr;
			}
		}

		if (platform.GetState() == EMPTY) {
			for (auto train : trains) {
				if (train == platform.processedTrain) continue;

				if (train->GetSpeed() > 0.0f && !train->GetCarStates().empty()) {
					auto& engineState = train->GetCarStates()[0];

					if (engineState.currentRail == platform.GetRail()) {
						bool shouldStop = false;
						if (engineState.traversingForward) {
							if (engineState.t > 0.85f) {
								shouldStop = true;
							}
						}
						else {
							if (engineState.t < 0.15f) {
								shouldStop = true;
							}
						}

						if (shouldStop) {
							// Stop the train
							train->SetBaseSpeed(train->GetSpeed());
							train->SetSpeed(0.0f);
							train->SetBusy(true);

							platform.SetCurrentTrain(train);
							platform.processedTrain = train;

							// Decide state based on resources accepted by the train
							ResourceType trainResourceType = train->GetCarriedResourceType();
							if (CheckResourceTypeInAccepted(trainResourceType) &&
								availableResources[static_cast<int>(trainResourceType)] < resourceMax) {
								platform.SetState(UNLOADING);
							}
							else if (CheckResourceTypeInProduction(trainResourceType) &&
								availableResources[static_cast<int>(trainResourceType)] > 0) {
								platform.SetState(LOADING);
							}
							else {
								platform.SetState(LOADING); // Default to loading if no action possible
							}

							platform.timer = 2.0f;
							break;
						}
					}
				}
			}
		}
		else {
			// Platform is busy (LOADING or UNLOADING)
			platform.timer -= deltaTime;
			if (platform.timer <= 0.0f) {
				if (platform.GetState() == LOADING) {
					Load();
				}
				else if (platform.GetState() == UNLOADING) {
					Unload();
				}
				// Release the train
				Train* train = platform.GetCurrentTrain();
				if (train) {
					train->SetSpeed(train->GetBaseSpeed());
					train->SetBusy(false);
					if (platform.GetTurnAroundAllowed()) {
						train->PlaceOnRail(platform.GetRail(), 
							1 - train->GetCarStates()[0].t, 
							!train->GetCarStates()[0].traversingForward);
					}
				}

				platform.SetCurrentTrain(nullptr);
				platform.SetState(EMPTY);
			}
		}
	}
}

void Station::RenderStation1(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	float stationScale = 4.0f;

	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, position - glm::vec3(0, position.y - stationScale / 2, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(stationScale));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	commonMeshes::getMesh("station1_base")->Render();

	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, position - glm::vec3(0, position.y - stationScale / 2, 0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, stationScale / 2.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(stationScale));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.3f, 0.8f, 1.3f));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	commonMeshes::getMesh("station1_roof")->Render();
}

void Station::RenderStation2(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	float stationScale = 4.0f;

	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, position - glm::vec3(0, position.y - stationScale / 2, 0));
	modelMatrix = glm::rotate(modelMatrix, -glm::radians(rotationAngle), glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(stationScale));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f, 1.0f, 1.0f));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	commonMeshes::getMesh("station2_base")->Render();

	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, position - glm::vec3(0, position.y - stationScale / 2, 0));
	modelMatrix = glm::rotate(modelMatrix, -glm::radians(rotationAngle), glm::vec3(0, 1, 0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, stationScale / 2.0f + 0.4f, 0.0f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(stationScale));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.8f, 1.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 0.2f, 2.0f));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	commonMeshes::getMesh("station2_roof")->Render();

	float pillarOffsets[2] = { -2.0f, 2.0f };

	for (float offset : pillarOffsets) {
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, position - glm::vec3(0, position.y - stationScale / 2, 0));
		modelMatrix = glm::rotate(modelMatrix, -glm::radians(rotationAngle), glm::vec3(0, 1, 0));

		modelMatrix = glm::translate(modelMatrix, glm::vec3(offset, 0.0f, 4.0f));

		modelMatrix = glm::scale(modelMatrix, glm::vec3(stationScale));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		commonMeshes::getMesh("station2_pillar")->Render();
	}
}

void Station::Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (!mesh) return;

	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (design == 1) {
		RenderStation1(shader, viewMatrix, projectionMatrix);
	}
	else if (design == 2) {
		RenderStation2(shader, viewMatrix, projectionMatrix);
	}
	else {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f, 3.0f, 3.0f)); // Make station visible
		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		mesh->Render();
	}

	glm::vec3 upOffset = glm::vec3(0.0f, 3.0f, 0.0f);

	// Render recepies
	for (Resource* resource : productionResources) {
		if (resource->HasRecepie()) {
			int recepieSize = 0;
			for (int amount : resource->GetRecepieAmounts()) {
				recepieSize += amount;
			}
			float start = (recepieSize / 2.0f) * 2.0;
			int cnt = 0;
			for (int i = 0; i < resource->GetRecepie().size(); i++) {
				std::string resName = ResourceTypeToString(resource->GetRecepie()[i]);
				Mesh* resMesh = commonMeshes::getMesh(resName);
				Resource* displayResource = new Resource(resMesh, resource->GetRecepie()[i]);

				for (int j = 0; j < resource->GetRecepieAmounts()[i]; j++) {
					glm::mat4 resourceModelMatrix = glm::mat4(1);
					resourceModelMatrix = glm::translate(resourceModelMatrix, position + glm::vec3(-start + cnt * 2.0f, 5.0f, 0.0f));
					resourceModelMatrix = glm::translate(resourceModelMatrix, upOffset);
					resourceModelMatrix = glm::scale(resourceModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
					displayResource->Render(shader, resourceModelMatrix, viewMatrix, projectionMatrix);
					cnt++;
				}
			}
			// Render resulting resource mesh
			glm::mat4 resultModelMatrix = glm::mat4(1);
			resultModelMatrix = glm::translate(resultModelMatrix, position + glm::vec3(-start + (cnt + 1) * 2.0f, 5.0f, 0.0f));
			resultModelMatrix = glm::translate(resultModelMatrix, upOffset);
			resultModelMatrix = glm::scale(resultModelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
			resource->Render(shader, resultModelMatrix, viewMatrix, projectionMatrix);
		}
		else {
			// Render the resource mesh above the station
			glm::mat4 resourceModelMatrix = glm::mat4(1);
			resourceModelMatrix = glm::translate(resourceModelMatrix, position + glm::vec3(0.0f, 5.0f, 0.0f));
			resourceModelMatrix = glm::translate(resourceModelMatrix, upOffset);
			resourceModelMatrix = glm::scale(resourceModelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
			resource->Render(shader, resourceModelMatrix, viewMatrix, projectionMatrix);
		}
	}

	// Render accepted resources
	for (int i = 0; i < acceptedResources.size(); i++) {
		Resource* resource = acceptedResources[i];
		int amount = availableResources[static_cast<int>(resource->GetType())];

		for (int j = 0; j < amount; j++) {
			glm::mat4 resourceModelMatrix = glm::mat4(1);
			resourceModelMatrix = glm::translate(resourceModelMatrix, position + glm::vec3(-2 + i * 4.0f, 8.0f + j * 1.2f, 0.0f));
			resourceModelMatrix = glm::translate(resourceModelMatrix, upOffset);
			resourceModelMatrix = glm::scale(resourceModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
			resource->Render(shader, resourceModelMatrix, viewMatrix, projectionMatrix);
		}
	}

	// Render production resources
	for (int i = 0; i < productionResources.size(); i++) {
		Resource* resource = productionResources[i];
		int amount = availableResources[static_cast<int>(resource->GetType())];

		for (int j = 0; j < amount; j++) {
			glm::mat4 resourceModelMatrix = glm::mat4(1);
			resourceModelMatrix = glm::translate(resourceModelMatrix, position + glm::vec3(2.0f - i * 4.0f, 8.0f + j * 1.2f, 0.0f));
			resourceModelMatrix = glm::translate(resourceModelMatrix, upOffset);
			resourceModelMatrix = glm::scale(resourceModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
			resource->Render(shader, resourceModelMatrix, viewMatrix, projectionMatrix);
		}
	}
}


void Station::Produce() {
	for (auto resource : productionResources) {
		ResourceType producedType = resource->GetType();
		if (availableResources[static_cast<int>(producedType)] >= resourceMax) {
			continue;
		}
		if (resource->HasRecepie()) {
			auto recepie = resource->GetRecepie();
			auto recepieAmounts = resource->GetRecepieAmounts();
			bool canProduce = true;
			for (size_t i = 0; i < recepie.size(); i++) {
				ResourceType rType = recepie[i];
				int rAmount = recepieAmounts[i];
				if (availableResources[static_cast<int>(rType)] < rAmount) {
					canProduce = false;
					break;
				}
			}
			if (canProduce) {
				for (size_t i = 0; i < recepie.size(); i++) {
					ResourceType rType = recepie[i];
					int rAmount = recepieAmounts[i];
					availableResources[static_cast<int>(rType)] -= rAmount;
				}
				availableResources[static_cast<int>(producedType)]++;
			}
		}
		else {
			availableResources[static_cast<int>(producedType)]++;
		}
	}
}

bool Station::CheckResourceTypeInProduction(ResourceType type) {
	for (auto resource : productionResources) {
		if (resource->GetType() == type) {
			return true;
		}
	}
	return false;
}

bool Station::CheckResourceTypeInAccepted(ResourceType type) {
	for (const auto& resource : acceptedResources) {
		if (resource->GetType() == type) {
			return true;
		}
	}
	return false;
}

void Station::Load() {
	for (auto& platform : platforms) {
		if (platform.GetState() != LOADING) continue;
		Train* train = platform.GetCurrentTrain();
		if (!train) continue;
		ResourceType trainResourceType = train->GetCarriedResourceType();
		if (!CheckResourceTypeInProduction(trainResourceType)) continue;
		for (auto& car: train->GetCars()) {
			if (!car->acceptingCargo || car->hasCargo) continue;
			if (availableResources[static_cast<int>(trainResourceType)] > 0) {
				car->hasCargo = true;
				availableResources[static_cast<int>(trainResourceType)]--;
			}
		}
	}
}

void Station::Unload() {
	for (auto& platform : platforms) {
		if (platform.GetState() != UNLOADING) continue;
		Train* train = platform.GetCurrentTrain();
		if (!train) continue;
		ResourceType trainResourceType = train->GetCarriedResourceType();
		if (!CheckResourceTypeInAccepted(trainResourceType)) continue;
		for (auto& car : train->GetCars()) {
			if (!car->hasCargo) continue;
			car->hasCargo = false;
			if (availableResources[static_cast<int>(trainResourceType)] < resourceMax) {
				availableResources[static_cast<int>(trainResourceType)]++;
			}
		}
	}
}
