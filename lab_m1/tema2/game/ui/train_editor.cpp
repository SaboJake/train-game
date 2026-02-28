#include "train_editor.h"

#include "lab_m1/tema2/my_utils/common_meshes.h"
#include "lab_m1/tema2/game/objects/train_engine.h"
#include "lab_m1/tema2/game/objects/train_car.h"

#include <iostream>

void TrainEditor::AddButtons() {
	Mesh* btnGreen = commonMeshes::getMesh("button_green");
	Mesh* btnRed = commonMeshes::getMesh("button_red");
	Mesh* btnYellow = commonMeshes::getMesh("button_yellow");

	removeCarButton = new Button(btnRed, glm::vec3(0), 1, 1);
	addCarButton = new Button(btnGreen, glm::vec3(0), 1, 1);
	confirmButton = new Button(btnGreen, glm::vec3(0), 1, 1);

	for (int i = 0; i < 6; i++) {
		selectResourceButtons.push_back(new Button(btnYellow, glm::vec3(0), 1, 1));
	}

	UpdateLayout();
}

void TrainEditor::OnResolutionChange(int windowWidth, int windowHeight) {
	float scale = (float)windowHeight / refRes.y;

	this->position = glm::vec3(windowWidth / 2.0f, windowHeight / 2.0f, position.z);

	this->width = (int)(400.0f * scale);
	this->height = (int)(200.0f * scale);
	this->res = glm::vec2(windowWidth, windowHeight);

	UpdateLayout();
}

void TrainEditor::UpdateLayout() {
	float scale = this->height / 200.0f;

	float mainBtnSize = 40.0f * scale;
	float resBtnSize = 30.0f * scale;
	float padding = 10.0f * scale;

	float controlRowY = position.y + (20.0f * scale);
	float offsetX = 60.0f * scale;

	removeCarButton->SetPosition(glm::vec3(position.x - offsetX, controlRowY, position.z + 1.0f));
	removeCarButton->SetSize((int)(mainBtnSize * 2), (int)mainBtnSize);

	addCarButton->SetPosition(glm::vec3(position.x + offsetX, controlRowY, position.z + 1.0f));
	addCarButton->SetSize((int)(mainBtnSize * 2), (int)mainBtnSize);

	float resourceRowY = position.y - (80.0f * scale);
	int numResources = selectResourceButtons.size();
	float totalResWidth = numResources * resBtnSize + (numResources - 1) * padding;
	float startX = position.x - totalResWidth / 2.0f + resBtnSize / 2.0f;

	for (int i = 0; i < numResources; i++) {
		selectResourceButtons[i]->SetPosition(glm::vec3(startX + i * (resBtnSize + padding), resourceRowY, position.z + 1.0f));
		selectResourceButtons[i]->SetSize((int)resBtnSize, (int)resBtnSize);
	}

	confirmButton->SetPosition(glm::vec3(position.x, position.y - (140.0f * scale), position.z + 1.0f));
	confirmButton->SetSize((int)(mainBtnSize * 4), (int)mainBtnSize);
}

TrainEditor::TrainEditor(Mesh* background,
	glm::vec3 position,
	int width,
	int height,
	glm::vec2 resolution,
	gfxc::TextRenderer* textRenderer
	)
	: background(background), position(position), width(width), height(height), res(resolution), textRenderer(textRenderer)
{
	std::string trainName = "Train_" + std::to_string(trainNr);
	displayedTrain = new Train(trainName);
	displayedTrain->AddCar(new TrainEngine("Engine", CreateTrainMeshes()));
	displayedTrain->AddCar(new TrainCar("Car1", CreateTrainCarMeshes()));
	displayedTrain->AddCar(new TrainCar("Car2", CreateTrainCarMeshes()));
	displayedTrain->SetCarriedResource(nullptr);

	disabledResourceButton = commonMeshes::getMesh("button_yellow");
	enabledResourceButton = commonMeshes::getMesh("button_green");

	AddButtons();
}

TrainEditor::TrainEditor(Mesh* background,
	glm::vec3 position,
	int width,
	int height,
	glm::vec2 resolution,
	gfxc::TextRenderer* textRenderer,
	Train* train)
	: TrainEditor(background, position, width, height, resolution, textRenderer)
{
	displayedTrain = train;
	AddButtons();
}

void TrainEditor::AddCar() {
	// Exclude the engine from the count
	if (displayedTrain->GetCars().size() - 1 < maxCars) {
		std::cout << "Cars before addition: " << displayedTrain->GetCars().size() - 1 << std::endl;
		std::cout << "Adding car" << std::endl;
		displayedTrain->AddCar(new TrainCar("Car" + std::to_string(displayedTrain->GetCars().size() + 1), CreateTrainCarMeshes()));
		std::cout << "Cars now: " << displayedTrain->GetCars().size() - 1 << std::endl;
	}
}

void TrainEditor::RemoveCar() {
	// Exclude the engine from the count
	if (displayedTrain->GetCars().size() - 1 > minCars) {
		std::cout << "Cars before removal: " << displayedTrain->GetCars().size() - 1 << std::endl;
		std::cout << "Removing car" << std::endl;
		displayedTrain->RemoveLastCar();
		std::cout << "Cars left: " << displayedTrain->GetCars().size() - 1 << std::endl;
	}
}

void TrainEditor::Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (!enabled) return;
	shader->Use();

	glm::mat4 bgModelMatrix = glm::mat4(1);
	bgModelMatrix = glm::translate(bgModelMatrix, position);
	bgModelMatrix = glm::scale(bgModelMatrix, glm::vec3(width, height, 1));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(bgModelMatrix));

	glm::mat4 identityView = glm::mat4(1);
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(identityView));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	background->Render();

	addCarButton->Render(shader, identityView, projectionMatrix);
	removeCarButton->Render(shader, identityView, projectionMatrix);
	confirmButton->Render(shader, identityView, projectionMatrix);

	for (auto button : selectResourceButtons) {
		button->Render(shader, identityView, projectionMatrix);
	}

	float scaling = res.y / refRes.y;

	float trainScale = 20.0f * scaling;
	float carSpacing = 5.5f * trainScale;
	auto cars = displayedTrain->GetCars();

	float totalTrainWidth = (cars.size() - 1) * carSpacing;
	float startX = position.x + totalTrainWidth / 2.0f;
	float trainY = position.y + 80.0f * scaling;

	for (size_t i = 0; i < cars.size(); i++) {
		cars[i]->SetPosition(glm::vec3(startX - i * carSpacing, trainY, position.z - trainScale / 2.0f));
		cars[i]->SetScale(trainScale);
		cars[i]->SetRotation(glm::vec3(0, glm::radians(180.0f), 0)); // 0 rotation for side view (flat)

		cars[i]->Render(shader, identityView, projectionMatrix);
	}

	float resMeshScale = 25.0f * scaling;

	for (int i = 0; i < selectResourceButtons.size(); i++) {
		float posX = selectResourceButtons[i]->GetPosition().x;
		float posY = selectResourceButtons[i]->GetPosition().y + (40.0f * scaling);

		ResourceType resType = static_cast<ResourceType>(i);
		std::string resName = ResourceTypeToString(resType);
		Resource* displayResource = ResourceFactory::CreateResourceByType(resType);
		if (displayResource) {
			glm::mat4 resModelMatrix = glm::mat4(1);
			resModelMatrix = glm::translate(resModelMatrix, glm::vec3(posX, posY, position.z));
			resModelMatrix = glm::scale(resModelMatrix, glm::vec3(resMeshScale, resMeshScale, resMeshScale));
			displayResource->Render(shader, resModelMatrix, identityView, projectionMatrix);
		}
	}
}

Train* TrainEditor::DeployTrain() {
	Train* deployedTrain = new Train("train_" + std::to_string(trainNr));
	deployedTrain->SetCarriedResource(
		ResourceFactory::CreateResourceByType(displayedTrain->GetCarriedResourceType()));

	deployedTrain->AddCar(new TrainEngine("Engine", CreateTrainMeshes()));
	for (size_t i = 1; i < displayedTrain->GetCars().size(); i++) {
		deployedTrain->AddCar(new TrainCar("Car" + std::to_string(i), CreateTrainCarMeshes()));
	}

	deployedTrain->SetSpeed(15.0f);
	deployedTrain->SetBaseSpeed(15.0f);

	deployment = false;
	return deployedTrain;
}

void TrainEditor::Use(int mouseX, int mouseY) {
	if (!enabled) return;
	glm::vec3 mousePos(mouseX, mouseY, 0);
	if (addCarButton->IsMouseOver(mousePos)) {
		AddCar();
	}
	if (removeCarButton->IsMouseOver(mousePos)) {
		RemoveCar();
	}

	if (confirmButton->IsMouseOver(mousePos)) {
		if (displayedTrain->GetCarriedResource() == nullptr) {
			std::cout << "Please select a resource type before confirming." << std::endl;
			return;
		}
		enabled = false;
		deployment = true;
		std::cout << "Train editing confirmed." << std::endl;
	}

	for (int i = 0; i < selectResourceButtons.size(); i++) {
		if (selectResourceButtons[i]->IsMouseOver(mousePos)) {
			Resource* r = ResourceFactory::CreateResourceByType(static_cast<ResourceType>(i));
			displayedTrain->SetCarriedResource(r);

			selectResourceButtons[i]->SetMesh(enabledResourceButton);
			for (int j = 0; j < selectResourceButtons.size(); j++) {
				if (j != i) {
					selectResourceButtons[j]->SetMesh(disabledResourceButton);
				}
			}
			std::cout << "Selected resource type: " << i << std::endl;
			break;
		}
	}
}
