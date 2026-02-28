#pragma once

#include "core/gpu/mesh.h"
#include "lab_m1/tema2/game/resource.h"
#include "lab_m1/tema2/game/objects/rail.h"
#include "lab_m1/tema2/game/objects/train.h"

#include <vector>

enum PlaformState {
	EMPTY,
	LOADING,
	UNLOADING
};

class Platform {
public:
	void SetRail(Rail* rail) {
		rail->deleteable = false;
		this->rail = rail; 
	}
	Rail* GetRail() { return rail; }

	PlaformState GetState() { return state; }
	void SetState(PlaformState newState) { this->state = newState; }
	Train* GetCurrentTrain() { return currentTrain; }
	void SetCurrentTrain(Train* train) { this->currentTrain = train; }

	float timer = 0.0f;
	Train* processedTrain = nullptr;
	bool GetTurnAroundAllowed() const { return turnAroundAllowed; }

private:
	Rail* rail;
	PlaformState state = EMPTY;
	Train* currentTrain = nullptr;

	bool isOccupied;
	bool turnAroundAllowed = true;
};

class Station {
public:
	Station(Mesh* mesh,
		glm::vec3 position,
		const std::vector<Platform>& platforms,
		const std::vector<Resource*>& productionResources,
		const std::vector<Resource*>& acceptedResources);

	void Update(float deltaTimeSeconds, const std::vector<Train*>& trains);
	void Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	void Produce();
	void Load();
	virtual void Unload();

	void SetPosition(const glm::vec3& position) { this->position = position; }
	glm::vec3 GetPosition() const { return position; }

	void SetRotationAngle(float angle) { this->rotationAngle = angle; }
	float GetRotationAngle() const { return rotationAngle; }

	void SetPlatforms(const std::vector<Platform>& platforms) { this->platforms = platforms; }
	std::vector<Platform>& GetPlatforms() { return platforms; }

	void SetDesign(int design) { this->design = design; }
	int GetDesign() const { return design; }

	Resource* GetMainProducedResource() {
		if (productionResources.empty()) return nullptr;
		return productionResources[0];
	}
protected:
	Mesh* mesh;
	glm::vec3 position;
	float rotationAngle;

	std::vector<Platform> platforms;
	// Resources that the station currently has. availableRsources[enum] = amount
	std::vector<int> availableResources;
	// Resources that this station can produce. Trains may only load these resources
	std::vector<Resource*> productionResources;
	// Resources that this station can accept for unloading
	std::vector<Resource*> acceptedResources;
	// May only have a maximum of 10 units of each resource
	int resourceMax = 10;
	float productionTime = 5.0f; // Time in seconds to produce one unit of each resource
	float currentProductionTimer = 0.0f;

	bool CheckResourceTypeInProduction(ResourceType type);
	bool CheckResourceTypeInAccepted(ResourceType type);

	int design;

	void RenderStation1(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	void RenderStation2(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
};
