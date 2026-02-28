#pragma once

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "lab_m1/tema2/my_utils/object3D.h"
#include "lab_m1/tema2/game/objects/rail.h"
#include "lab_m1/tema2/game/objects/train_unit.h"
#include "lab_m1/tema2/game/resource.h"

struct CarTrackState {
	Rail* currentRail;
	float t;
	bool traversingForward;
	bool isMoving;
};

class Train {
public:
	Train(const std::string& name);
	Train(const Train& other);
	~Train();

	void Update(float deltaTimeSeconds);
	void Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	void AddCar(TrainUnit* car);
	void RemoveLastCar();
	void PlaceOnRail(Rail* rail, float progress, bool forward = true);

	float GetSpeed() const { return speed; }
	void SetSpeed(float speed) { this->speed = speed; }
	float GetBaseSpeed() const { return baseSpeed; }
	void SetBaseSpeed(float baseSpeed) { this->baseSpeed = baseSpeed; }

	int GetCarIndex(glm::vec3 rayOrigin, glm::vec3 rayDirection);
	float GetCarDistace(glm::vec3 rayOrigin, glm::vec3 rayDirection);

	bool CheckRail(Rail* rail);

	std::vector<TrainUnit*> GetCars() { return cars; }
	ResourceType GetCarriedResourceType() { return carriedResource->GetType(); }
	Resource* GetCarriedResource() { return carriedResource; }
	std::vector<CarTrackState>& GetCarStates() { return carStates; }
	void SetCarriedResource(Resource* resource) { carriedResource = resource; }

	bool IsBusy() const { return busy; }
	void SetBusy(bool value) { busy = value; }

private:
	std::string name;
	float speed;
	float baseSpeed;
	bool busy;

	std::vector<CarTrackState> carStates;
	std::vector<TrainUnit*> cars;

	void RenderPart(Mesh* mesh, Shader* shader, const glm::mat4& parentMatrix,
		glm::vec3 offset, glm::vec3 scale, glm::vec3 axisRot, float angleRot);

	void AdvanceState(CarTrackState& state, float distance);

	bool IsConnectedToStart(Rail* source, Rail* target);

	// A train may only carry one type of resource at a time
	Resource* carriedResource;
};
