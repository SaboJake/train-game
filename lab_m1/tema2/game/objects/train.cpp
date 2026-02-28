#include "train.h"
#include "lab_m1/tema2/my_utils/ray_utils.h"
#include <glm/gtc/matrix_transform.hpp>

Train::Train(const std::string& name)
	: name(name), speed(0.0f), busy(false)
{
}

Train::Train(const Train& other)
    : name(other.name), speed(other.speed), baseSpeed(other.baseSpeed), carriedResource(other.carriedResource), busy(false)
{
    // Deep copy of cars
    for (const auto& car : other.cars) {
        cars.push_back(new TrainUnit(*car));
    }

    // Copy car states
    carStates = other.carStates;
}

Train::~Train()
{
    for (auto car : cars) {
        delete car;
    }
	delete carriedResource;
    cars.clear();
}

void Train::AddCar(TrainUnit* car) {
    cars.push_back(car);
    CarTrackState newState = { nullptr, 0.0f, true, false };
    if (carStates.size() > 0) newState = carStates.back();
    carStates.push_back(newState);
}

void Train::RemoveLastCar() {
    if (cars.empty()) return;
    cars.pop_back();
    carStates.pop_back();
}

void Train::PlaceOnRail(Rail* rail, float progress, bool forward) {
    if (cars.empty()) return;

    carStates[0].currentRail = rail;
    carStates[0].t = progress;
    carStates[0].traversingForward = forward;
    carStates[0].isMoving = true;

    float currentDist = 0.0f;
    Rail* currentRail = rail;
    float railLength = currentRail->GetLength();

    for (size_t i = 1; i < carStates.size(); i++) {
        float spacing = 4.5f;
        float tOffset = spacing / railLength;

        carStates[i] = carStates[i - 1];
        carStates[i].t -= carStates[i].traversingForward ? tOffset : -tOffset;

        while (carStates[i].t < 0.0f || carStates[i].t > 1.0f) {
            float overflow = carStates[i].t < 0.0f
                ? -carStates[i].t * railLength
                : (carStates[i].t - 1.0f) * railLength;

            if (carStates[i].t < 0.0f && !currentRail->connectedStart.empty()) {
                int index = currentRail->activeSwitchStart;
                if (index >= currentRail->connectedStart.size()) index = 0;
                currentRail = currentRail->connectedStart[index];
                railLength = currentRail->GetLength();
                carStates[i].currentRail = currentRail;
                carStates[i].t = 1.0f - (overflow / railLength);
                carStates[i].traversingForward = true;
            }
            else if (carStates[i].t > 1.0f && !currentRail->connectedEnd.empty()) {
                int index = currentRail->activeSwitchEnd;
                if (index >= currentRail->connectedEnd.size()) index = 0;
                currentRail = currentRail->connectedEnd[index];
                railLength = currentRail->GetLength();
                carStates[i].currentRail = currentRail;
                carStates[i].t = overflow / railLength;
                carStates[i].traversingForward = false;
            }
            else {
                carStates[i].t = carStates[i].t < 0.0f ? 0.0f : 1.0f;
                carStates[i].isMoving = false;
                break;
            }
        }
    }
}

bool Train::CheckRail(Rail* rail) {
    for (const auto& state : carStates) {
        if (state.currentRail == rail) {
            return true;
        }
    }
    return false;
}

bool Train::IsConnectedToStart(Rail* source, Rail* target) {
    for (auto r : target->connectedStart) if (r == source) return true;
    return false;
}

void Train::Update(float deltaTimeSeconds) {
	if (speed == 0.0f) return;

    if (!carStates.empty() && !carStates[0].isMoving) {
        for (size_t i = 0; i < carStates.size(); i++) {
            carStates[i].isMoving = false;
        }
        return;
	}

	float moveDistance = speed * deltaTimeSeconds;
    for (size_t i = 0; i < carStates.size(); i++) {
        AdvanceState(carStates[i], moveDistance);
        // Update the position and rotation of the corresponding car
        if (carStates[i].currentRail) {
            glm::vec3 newPos = carStates[i].currentRail->GetPos(carStates[i].t);
            newPos.y += 0.6f;
			cars[i]->SetPosition(newPos);

            glm::vec3 forward = carStates[i].currentRail->GetForwardVector(carStates[i].t);
            if (!carStates[i].traversingForward) {
				forward = -forward;
            }
			float angleY = atan2(forward.x, forward.z);
			cars[i]->SetRotation(glm::vec3(0.0f, glm::degrees(angleY) - 90.0f, 0.0f));
        }
	}
}

void Train::AdvanceState(CarTrackState& state, float distance) {
    if (!state.currentRail) return;

    float railLength = state.currentRail->GetLength();
    if (railLength < 0.01f) return;

    float tIncrement = distance / railLength;
    state.t += state.traversingForward ? tIncrement : -tIncrement;

    // Exiting at end
    if (state.t >= 1.0f) {
        float overflow = (state.t - 1.0f) * railLength;
        if (state.currentRail->connectedEnd.empty()) {
            state.t = 1.0f;
            state.isMoving = false;
            return;
        }

        int index = state.currentRail->activeSwitchEnd;
        if (index >= state.currentRail->connectedEnd.size()) {
            index = 0;
        }
        Rail* nextRail = state.currentRail->connectedEnd[index];
        if (IsConnectedToStart(state.currentRail, nextRail)) {
            state.t = overflow / nextRail->GetLength();
            state.traversingForward = true;
        }
        else {
            state.t = 1.0f - (overflow / nextRail->GetLength());
            state.traversingForward = false;
		}
        state.currentRail = nextRail;
    }
    // Exiting at start
    else if (state.t <= 0.0f) {
        float overflow = -state.t * railLength;
        if (state.currentRail->connectedStart.empty()) {
            state.t = 0.0f;
            state.isMoving = false;
            return;
        }
        int index = state.currentRail->activeSwitchStart;
        if (index >= state.currentRail->connectedStart.size()) {
            index = 0;
        }
        Rail* nextRail = state.currentRail->connectedStart[index];
        if (IsConnectedToStart(state.currentRail, nextRail)) {
            state.t = overflow / nextRail->GetLength();
            state.traversingForward = true;
        }
        else {
            state.t = 1.0f - (overflow / nextRail->GetLength());
            state.traversingForward = false;
        }
        state.currentRail = nextRail;
    }
}

int Train::GetCarIndex(glm::vec3 rayOrigin, glm::vec3 rayDirection) {
    float closest = 9999.0f;
    int index = -1;

    for (size_t i = 0; i < cars.size(); i++) {
        float dist = RayUtils::IntersectSphere(rayOrigin, rayDirection, cars[i]->GetPosition(), 2.5f);
        if (dist >= 0.0f && dist < closest) {
            closest = dist;
            index = static_cast<int>(i);
        }
    }
    return index;
}

float Train::GetCarDistace(glm::vec3 rayOrigin, glm::vec3 rayDirection) {
    float closest = 9999.0f;
	float distance = -1.0f;

    for (size_t i = 0; i < cars.size(); i++) {
        float dist = RayUtils::IntersectSphere(rayOrigin, rayDirection, cars[i]->GetPosition(), 2.5f);
        if (dist >= 0.0f && dist < closest) {
            closest = dist;
            distance = dist;
        }
    }
    return distance;
}

void Train::Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	// Render resource above the first car
	glm::mat4 modelMatrix = glm::mat4(1);
    if (carriedResource && !cars.empty()) {
        modelMatrix = glm::translate(modelMatrix, cars[0]->GetPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
        carriedResource->Render(shader, modelMatrix, viewMatrix, projectionMatrix);
    }
    for (auto car : cars) {
        car->Render(shader, viewMatrix, projectionMatrix);
        if (car->hasCargo) {
            // Render resource above the car
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, car->GetPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
            carriedResource->Render(shader, modelMatrix, viewMatrix, projectionMatrix);
		}
	}
}
