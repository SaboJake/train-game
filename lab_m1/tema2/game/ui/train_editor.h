#pragma once

#include <glm/glm.hpp>

#include "components/text_renderer.h"
#include "core/gpu/mesh.h"
#include "lab_m1/tema2/game/objects/train.h"
#include "lab_m1/tema2/game/ui/button.h"


class TrainEditor {
public:
	TrainEditor(Mesh* background,
		glm::vec3 position,
		int width,
		int height,
		glm::vec2 resolution,
		gfxc::TextRenderer* textRenderer);
	TrainEditor(Mesh* background,
		glm::vec3 position,
		int width,
		int height,
		glm::vec2 resolution,
		gfxc::TextRenderer* textRenderer,
		Train* train);

	void Render(Shader* shader, const glm::mat4& modelMatrix, const glm::mat4& projectionMatrix);

	void AddCar();
	void RemoveCar();

	void Toggle() { enabled = !enabled; }
	bool IsEnabled() const { return enabled; }

	void Use(int mouseX, int mouseY);
	void ResetTrain();

	bool IsReadyForDeployment() const { return deployment; }
	void SetDeployment(bool value) { deployment = value; }

	bool IsTrainEditMode() const { return enabled; }

	Train* DeployTrain();

	void OnResolutionChange(int windowWidth, int windowHeight);

private:
	const glm::vec2 refRes = glm::vec2(1280, 720);

	glm::vec2 res;

	bool enabled = false;
	bool deployment = false;
	Mesh* background;
	glm::vec3 position;
	int width;
	int height;

	gfxc::TextRenderer* textRenderer;

	Train* displayedTrain;
	Button* addCarButton;
	Button* removeCarButton;
	Button* confirmButton;

	std::vector<Button*> selectResourceButtons;
	Mesh* disabledResourceButton;
	Mesh* enabledResourceButton;

	int maxCars = 5;
	int minCars = 1;

	int trainNr = 0;

	void AddButtons();
	void UpdateLayout();
};
