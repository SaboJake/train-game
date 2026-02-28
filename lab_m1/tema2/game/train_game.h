#pragma once

#include <vector>

#include "components/simple_scene.h"
#include "components/text_renderer.h"
#include "lab_m1/tema2/game/objects/train.h"
#include "lab_m1/tema2/game/objects/train_car.h"
#include "lab_m1/tema2/game/objects/train_engine.h"
#include "lab_m1/tema2/game/objects/rail.h"
#include "lab_m1/tema2/game/rail_manager.h"
#include "lab_m1/tema2/game/objects/ground.h"
#include "lab_m1/tema2/game/objects/station.h"
#include "lab_m1/tema2/game/objects/central_station.h"
#include "lab_m1/tema2/game/ui/train_editor.h"
#include "lab_m1/tema2/game/camera.h"
#include "lab_m1/tema2/game/ui/stats.h"

namespace m1
{
    class TrainGame : public gfxc::SimpleScene
    {
    public:
        TrainGame();
        ~TrainGame();

        void Init() override;
    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

		implemented::MyCamera* camera;

        int L;

		Train* train;

		TrainEngine* trainEngine;
		TrainCar* trainCar1;
		TrainCar* trainCar2;
		std::vector<Station*> stations;

		CentralStation* centralStation;

		RailManager* railManager;

		glm::vec3 groundMouseIntersectionPoint;

        int selectedRailIndex;
        float selectedRailT;
		int selectedTrainIndex;

        std::vector<Ground*> groundTiles;

		TrainEditor* trainEditor;

		Button* testButton;

        bool movingCamera;
		bool changingTrackSwitch;

		gfxc::TextRenderer* textRenderer;

		void InitGroundTiles();
		void InitStations();
		void InitMeshesAndShaders();
        void InitCamera();
        void CreatePlatformsForStation(Station* station, int count, float angle);

		float worldLimitMinX;
		float worldLimitMaxX;
		float worldLimitMinZ;
        float worldLimitMaxZ;

		void LimitPoint(glm::vec3& point);

		void RenderMinimap(float deltaTimeSeconds);

        float uiScale;
		bool minimapEnabled;

        Stats* statsScreen;
    };
}   // namespace m1
