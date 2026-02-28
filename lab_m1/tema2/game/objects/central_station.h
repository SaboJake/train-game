#pragma once

#include "lab_m1/tema2/game/objects/station.h"
#include "lab_m1/tema2/my_utils/requests.h"

class CentralStation : public Station {
public:
	// Centrall station may only request resources, not produce any
	CentralStation(Mesh* mesh,
		glm::vec3 position,
		const std::vector<Platform>& platforms);

	void Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	// Additionally check if the request is fulfilled
	void Unload() override;

	int GetOrdersFulfilled() const { return ordersFulfilled; }
	void GetNextRequest();
private:
	// Resources that this station is requesting. requestedResources[enum] = amount
	std::vector<int> requestedResources;

	int ordersFulfilled;
	RequestManager* requestManager;

	void RenderStation(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
};
