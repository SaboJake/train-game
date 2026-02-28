#include "lab_m1/tema2/game/train_game.h"
#include "lab_m1/tema2/my_utils/object3D.h"
#include "lab_m1/tema2/game/objects/train.h"
#include "lab_m1/tema2/my_utils/common_meshes.h"
#include "lab_m1/tema2/my_utils/ray_utils.h"

#include "components/camera.h"
#include "components/transform.h"

#include <vector>
#include <string>
#include <iostream>

#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_RIGHT 2

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

TrainGame::TrainGame()
{
}


TrainGame::~TrainGame()
{
}

void TrainGame::LimitPoint(glm::vec3& point) {
    if (point.x < worldLimitMinX) point.x = worldLimitMinX;
    if (point.x > worldLimitMaxX) point.x = worldLimitMaxX;
    if (point.z < worldLimitMinZ) point.z = worldLimitMinZ;
    if (point.z > worldLimitMaxZ) point.z = worldLimitMaxZ;
}

void TrainGame::CreatePlatformsForStation(Station* station, int count, float angle) {
    glm::vec3 pos = station->GetPosition();
	pos.y = 0.0f;

    float spacing = 3.0f;
    float platformLength = 30.0f;
	float stationClearance = 6.0f; // Distance from station center to first platform

    vector<Platform> platforms;

    float rads = glm::radians(angle);

    glm::vec3 railDir = glm::vec3(cos(rads), 0.0f, sin(rads));

    glm::vec3 offsetDir = glm::vec3(-sin(rads), 0.0f, cos(rads));

    for (int i = 0; i < count; i++)
    {
        Platform platform;

        float currentOffset = i * spacing + stationClearance;

        glm::vec3 platformCenter = pos + (offsetDir * currentOffset);

        glm::vec3 startPos = platformCenter - (railDir * (platformLength * 0.5f));
        glm::vec3 endPos = platformCenter + (railDir * (platformLength * 0.5f));

        StraightRail* straightRail = new StraightRail(startPos, endPos);
		railManager->AddRail(straightRail);
        platform.SetRail(straightRail);

        platforms.push_back(platform);
    }

    station->SetPlatforms(platforms);
}

void TrainGame::InitStations() {
    float offsetY = 1.5f;
    // central station
    centralStation = new CentralStation(
        meshes["cube"],
        glm::vec3(0.0f, offsetY, 0.0f),
        {}
    );
    CreatePlatformsForStation(centralStation, 3, 0.0);

    // wood 
	vector<Resource*> productionWood = { ResourceFactory::CreateWoodResource() };
    Station* woodStation = new Station(
        meshes["cube"],
        glm::vec3(-60.0f, offsetY, 100.0f),
        {},
        productionWood,
        {}
	);
	woodStation->SetDesign(1);
	woodStation->SetRotationAngle(135.0f);
	CreatePlatformsForStation(woodStation, 2, 135.0f);
	stations.push_back(woodStation);

    // iron mine
	vector<Resource*> productionIron = { ResourceFactory::CreateIronResource() };
    Station* ironMine = new Station(
        meshes["cube"],
        glm::vec3(-80.0f, offsetY, 20.0f),
        {},
        productionIron,
        {}
    );
	ironMine->SetDesign(1);
	ironMine->SetRotationAngle(45.0f);
	CreatePlatformsForStation(ironMine, 2, 45.0f);
	stations.push_back(ironMine);

	// planks factory
	vector<Resource*> productionPlanks = { ResourceFactory::CreatePlanksResource() };
	vector<Resource*> requiredWood = { ResourceFactory::CreateWoodResource() };
    Station* planksFactory = new Station(
        meshes["cube"],
        glm::vec3(36.0f, offsetY, 108.0f),
        {},
        productionPlanks,
        requiredWood
	);
	planksFactory->SetDesign(2);
	planksFactory->SetRotationAngle(90.0f);
	CreatePlatformsForStation(planksFactory, 2, 90.0f);
	stations.push_back(planksFactory);

	// nails factory
	vector<Resource*> productionNails = { ResourceFactory::CreateNailsResource() };
	vector<Resource*> requiredIronForNails = { ResourceFactory::CreateIronResource() };
	Station* nailsFactory = new Station(
        meshes["cube"],
        glm::vec3(52.0f, offsetY, -40.0f),
        {},
        productionNails,
		requiredIronForNails
	);
	nailsFactory->SetDesign(2);
	nailsFactory->SetRotationAngle(0.0f);
	CreatePlatformsForStation(nailsFactory, 2, 0.0f);
	stations.push_back(nailsFactory);

	// steel mill
	vector<Resource*> productionSteel = { ResourceFactory::CreateSteelResource() };
	vector<Resource*> requiredIronForSteel = { ResourceFactory::CreateIronResource() };
    Station* steelMill = new Station(
        meshes["cube"],
        glm::vec3(80.0f, offsetY, -100.0f),
        {},
        productionSteel,
        requiredIronForSteel
	);
	steelMill->SetDesign(2);
	steelMill->SetRotationAngle(90.0f);
	CreatePlatformsForStation(steelMill, 2, 90.0f);
	stations.push_back(steelMill);

	// door factory
	vector<Resource*> productionDoors = { ResourceFactory::CreateDoorsResource() };
    vector<Resource*> requiredResourcesForDoors = {
        ResourceFactory::CreatePlanksResource(),
        ResourceFactory::CreateNailsResource()
	};
    Station* doorFactory = new Station(
        meshes["cube"],
        glm::vec3(10.0f, offsetY, -48.0f),
        {},
        productionDoors,
        requiredResourcesForDoors
	);
	doorFactory->SetDesign(2);
	doorFactory->SetRotationAngle(180.0f);
	CreatePlatformsForStation(doorFactory, 2, 180.0f);
	stations.push_back(doorFactory);
}

void TrainGame::InitGroundTiles() {
	int gridSize = 64;
	int halfGridSize = gridSize / 2;
    vector<vector<int>> grid(gridSize, vector<int>(gridSize));

    grid = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 2, 2, 2, 2, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            Ground* groundTile = new Ground(glm::vec3((i - halfGridSize) * 4, 0, (j - halfGridSize) * 4), grid[i][j]);
            groundTiles.push_back(groundTile);
        }
	}
}

void TrainGame::InitCamera() {
    camera = new implemented::MyCamera();
    camera->Set(glm::vec3(0, 30, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    float fov = RADIANS(60);
    camera->setFov(fov);
    float zNear = 0.01f;
    camera->setzNear(zNear);
    float zFar = 200.0f;
    camera->setzFar(zFar);
    camera->SetProjectionMatrix(glm::perspective(fov, window->props.aspectRatio, zNear, zFar));
}

void TrainGame::InitMeshesAndShaders() {
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Shader* shader = new Shader("Highlight");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "game", "shaders", "highlight_vertex.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "game", "shaders", "highlight_fragment.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    Mesh* cube = object3D::CreateCube("cube", glm::vec3(0, 0, 0), 1.0f, glm::vec3(1, 0, 0), true);
    Mesh* sphere = object3D::CreateSphere("sphere", glm::vec3(0, 0, 0), 0.5f, glm::vec3(0, 1, 0), true, 20, 20);
    Mesh* cone = object3D::CreateCone("cone", glm::vec3(0, 0, 0), 0.5f, 1.0f, glm::vec3(0, 0, 1), true, 20);
    Mesh* cylinder = object3D::CreateCylinder("cylinder", glm::vec3(0, 0, 0), 0.5f, 1.0f, glm::vec3(1, 1, 0), true, 20);
    Mesh* halfCylinder = object3D::CreateHollowHalfCylinder("halfCylinder", glm::vec3(0, 0, 0), 0.75f, 0.5f, 1.0f, glm::vec3(1, 0, 1), true, 20);
    Mesh* quad = object3D::CreateQuad("quad", glm::vec3(0, 0, 0), 1.0f, 1.0f, glm::vec3(0, 1, 0), true);
    Mesh* buttonMesh = object3D::CreateRectangle("button", glm::vec3(0, 0, 0), 1.0f, 1.0f, glm::vec3(0.0f, 0.5f, 0.8f), true);
    Mesh* line = object3D::CreateQuad("mm_line", glm::vec3(0), 1.0f, 1.0f, glm::vec3(0.8f, 0.8f, 0.8f), true);

    Mesh* trainEditorBackGround = object3D::CreateRectangle("trainEditorBackGround", glm::vec3(0, 0, 0), 2.0f, 2.0f, glm::vec3(0.3f, 0.3f, 0.5f), true);

    vector<Mesh*> trainMeshes = CreateTrainMeshes();
    vector<Mesh*> trainCarMeshes = CreateTrainCarMeshes();

    AddMeshToList(cube);
    AddMeshToList(sphere);
    AddMeshToList(cone);
    AddMeshToList(cylinder);
    AddMeshToList(halfCylinder);
    AddMeshToList(quad);
    AddMeshToList(buttonMesh);
    AddMeshToList(line);
	AddMeshToList(trainEditorBackGround);

    std::vector<std::string> resNames = { "wood", "iron", "planks", "nails", "steel", "doors" };
    std::vector<glm::vec3> resColors = {
        glm::vec3(0.6f, 0.4f, 0.2f), // Wood
        glm::vec3(0.5f, 0.5f, 0.5f), // Iron
        glm::vec3(0.7f, 0.5f, 0.3f), // Planks
        glm::vec3(0.9f, 0.9f, 0.9f), // Nails
        glm::vec3(0.6f, 0.6f, 0.6f), // Steel
        glm::vec3(0.5f, 0.3f, 0.1f)  // Doors
    };

    for (int i = 0; i < resNames.size(); i++) {
        Mesh* quad = object3D::CreateQuad("mm_station_" + resNames[i], glm::vec3(0, 0, 0), 1.0f, 1.0f, resColors[i], true);
        AddMeshToList(quad);
        Mesh* arrow = object3D::CreateTriangle("mm_train_" + resNames[i], glm::vec3(0, 0, 0), 1.0f, resColors[i], true);
        AddMeshToList(arrow);
    }
}

void TrainGame::Init() {
	InitCamera();

    worldLimitMinX = -130.0f;
    worldLimitMaxX = 126.0f;
    worldLimitMinZ = -130.0f;
    worldLimitMaxZ = 126.0f;

	commonMeshes::InitMeshes();
    InitMeshesAndShaders();

	groundMouseIntersectionPoint = glm::vec3(0);
	selectedRailIndex = -1;
	selectedRailT = -1.0f;
	selectedTrainIndex = -1;
    changingTrackSwitch = false;

	railManager = new RailManager();
    
	InitGroundTiles();

    railManager->SetGroundTiles(groundTiles);

    for (auto rail : railManager->GetRails()) {
		railManager->ComputeCosmeticsForRail(rail);
    }
    
	InitStations();

    uiScale = 1.0f;
    minimapEnabled = true;
    glm::ivec2 resolution = window->GetResolution();

	textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);
    textRenderer->Load("assets/fonts/Hack-Bold.ttf", 40);

	trainEditor = new TrainEditor(meshes["trainEditorBackGround"], glm::vec3(resolution.x / 2, resolution.y / 2, 0), 400, 200, resolution, textRenderer);
    
    statsScreen = new Stats(textRenderer);
}

void TrainGame::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();

    glViewport(0, 0, resolution.x, resolution.y);
	camera->UpdateProjectionMatrix(window->props.aspectRatio);
}

void TrainGame::RenderMinimap(float deltaTimeSeconds) {
	glm::ivec2 resolution = window->GetResolution();

    int mapSize = 300 * uiScale;
    int margin = 20 * uiScale;
	glViewport(resolution.x - mapSize - margin, resolution.y - mapSize - margin, mapSize, mapSize);
	glClear(GL_DEPTH_BUFFER_BIT);

	float orthoSize = 140.0f;
    glm::mat4 projectionMatrix = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.0f, 200.0f);
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 100, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));

	Shader* shader = shaders["VertexColor"];

    for (auto& tile : groundTiles) {
        tile->Render(shader, viewMatrix, projectionMatrix);
    }

    for (auto rail : railManager->GetRails()) {
		rail->RenderMinimap(shader, viewMatrix, projectionMatrix);
    }

    for (auto station : stations) {
		Resource* res = station->GetMainProducedResource();
        if (!res) continue;
		string meshName = "mm_station_" + ResourceTypeToString(res->GetType());

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, station->GetPosition());
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 10.0f, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 1.0f, 10.0f));
        
        shader->Use();
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        meshes[meshName]->Render();
	}

    {
		// Central Station
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, centralStation->GetPosition());
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 10.0f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(15.0f, 1.0f, 15.0f));
        
        shader->Use();
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        meshes["cube"]->Render();
    }

    for (auto train : railManager->GetTrains()) {
		if (train->GetCars().empty()) continue;
		if (!train->GetCarriedResource()) continue;

		TrainUnit* engine = train->GetCars()[0];
		glm::vec3 pos = engine->GetPosition();
        glm::vec3 rot = engine->GetRotation();

		string meshName = "mm_train_" + ResourceTypeToString(train->GetCarriedResource()->GetType());

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, pos);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 15.0f, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.y), glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(7.0f, 1.0f, 10.5f));

        shader->Use();
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		meshes[meshName]->Render();
    }

	glViewport(0, 0, resolution.x, resolution.y);
}

void TrainGame::Update(float deltaTimeSeconds)
{
	// Update Stats Screen
	statsScreen->SetScore(centralStation->GetOrdersFulfilled() * 10);
    statsScreen->Update(deltaTimeSeconds);
    
    // Update Station
    for (Station* station : stations) {
        station->Update(deltaTimeSeconds, railManager->GetTrains());
        station->Render(shaders["VertexColor"], camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }
    centralStation->Update(deltaTimeSeconds, railManager->GetTrains());
    centralStation->Render(shaders["VertexColor"], camera->GetViewMatrix(), camera->GetProjectionMatrix());
    
    // Update Trains
    for (int i = 0; i < railManager->GetTrains().size(); i++) {
        Train* train = railManager->GetTrains()[i];
        train->Update(deltaTimeSeconds);

        Shader* sh = (i == selectedTrainIndex) ? shaders["Highlight"] : shaders["VertexColor"];
        train->Render(sh, camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }
    
	// Update Rails
    int trackSwitching = 0;
    if (changingTrackSwitch && selectedRailT >= 0 && selectedRailT <= 0.5) {
        trackSwitching = -1;
    }
    else if (changingTrackSwitch && selectedRailT > 0.5 && selectedRailT <= 1.0) {
        trackSwitching = 1;
	}

	railManager->RenderAllRails(shaders["VertexColor"],
        shaders["Highlight"],
        selectedRailIndex,
        trackSwitching,
        camera->GetViewMatrix(), camera->GetProjectionMatrix());

	// Render Ground Tiles
    for (auto& groundTile : groundTiles) {
        groundTile->Render(shaders["VertexColor"], camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }

    // Render UI
    glm::ivec2 resolution = window->GetResolution();

    glm::mat4 ortho = glm::ortho(0.0f, (float)resolution.x, 0.0f, (float)resolution.y, -200.0f, 200.0f);
    glDisable(GL_DEPTH_TEST);

    if (minimapEnabled) {
        RenderMinimap(deltaTimeSeconds);
    }

	trainEditor->Render(shaders["VertexColor"], glm::mat4(1), ortho);

    glEnable(GL_DEPTH_TEST);
}

void TrainGame::FrameEnd()
{
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void TrainGame::OnInputUpdate(float deltaTime, int mods)
{
    // Add key press event
	camera->OnInputUpdate(window, deltaTime, mods);
	railManager->SetGridSnapping(window->KeyHold(GLFW_KEY_LEFT_SHIFT) || window->KeyHold(GLFW_KEY_RIGHT_SHIFT));

	movingCamera = window->MouseHold(GLFW_MOUSE_BUTTON_2);
}


void TrainGame::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_N) {
        centralStation->GetNextRequest();
    }

    if (key == GLFW_KEY_T && !railManager->IsBuildMode()) {
		trainEditor->Toggle();
    }
    if (key == GLFW_KEY_M) {
        minimapEnabled = !minimapEnabled;
    }

    if (key == GLFW_KEY_B && !trainEditor->IsTrainEditMode()) {
        railManager->ToggleBuildMode();
		railManager->SetStartSelected(false);
		railManager->SetEndSelected(false);
        if (railManager->IsBuildMode()) {
            std::cout << "Build mode ON" << endl;
        }
        else {
            std::cout << "Build mode OFF" << endl;
        }
    }
    if (key == GLFW_KEY_C) {
        if (railManager->IsBuildMode()) {
            railManager->ToggleCurvedMode();
            if (railManager->IsCurvedMode()) {
                std::cout << "Curved rail mode ON" << endl;
            }
            else {
                std::cout << "Curved rail mode OFF" << endl;
            }
			railManager->UpdatePreviewRail(groundMouseIntersectionPoint);
        }
        else {
            if (selectedRailIndex != -1) {
                changingTrackSwitch = !changingTrackSwitch;
            }
        }
	}

    if (key == GLFW_KEY_A && !movingCamera) {
        if (railManager->IsBuildMode() && railManager->IsCurvedMode()) {
            railManager->DecrementAngle();
			railManager->UpdatePreviewRail(groundMouseIntersectionPoint);
        }
        else if (!railManager->IsBuildMode() && selectedRailIndex != -1 && changingTrackSwitch) {
			Rail* rail = railManager->GetRails()[selectedRailIndex];
            if (!railManager->CheckRailOccupied(rail)) {
                if (selectedRailT >= 0.0f && selectedRailT <= 0.5f && !rail->connectedStart.empty()) {
                    std::cout << "Switching start connection" << endl;
                    rail->activeSwitchStart--;
                    if (rail->activeSwitchStart < 0) {
                        rail->activeSwitchStart = rail->connectedStart.size() - 1;
                    }
                    std::cout << "New active start connection: " << rail->activeSwitchStart << endl;
                }
                else if (selectedRailT > 0.5f && selectedRailT <= 1.0f && !rail->connectedEnd.empty()) {
                    std::cout << "Switching end connection" << endl;
                    rail->activeSwitchEnd--;
                    if (rail->activeSwitchEnd < 0) {
                        rail->activeSwitchEnd = rail->connectedEnd.size() - 1;
                    }
                    std::cout << "New active end connection: " << rail->activeSwitchEnd << endl;
                }
            }
            else {
                std::cout << "Cannot switch track: rail is occupied by a train!" << endl;
            }
		}
    }

    if (key == GLFW_KEY_D && !movingCamera) {
        if (railManager->IsBuildMode() && railManager->IsCurvedMode()) {
            railManager->IncrementAngle();
			railManager->UpdatePreviewRail(groundMouseIntersectionPoint);
        }
        else if (!railManager->IsBuildMode() && selectedRailIndex != -1 && changingTrackSwitch) {
            Rail* rail = railManager->GetRails()[selectedRailIndex];
            if (!railManager->CheckRailOccupied(rail)) {
                if (selectedRailT >= 0.0f && selectedRailT <= 0.5f && !rail->connectedStart.empty()) {
                    std::cout << "Switching start connection" << endl;
                    rail->activeSwitchStart++;
                    rail->activeSwitchStart = rail->activeSwitchStart % (rail->connectedStart.size());
                    std::cout << "New active start connection: " << rail->activeSwitchStart << endl;
                }
                else if (selectedRailT > 0.5f && selectedRailT <= 1.0f && !rail->connectedEnd.empty()) {
                    std::cout << "Switching end connection" << endl;
                    rail->activeSwitchEnd++;
                    rail->activeSwitchEnd = rail->activeSwitchEnd % (rail->connectedEnd.size());
                    std::cout << "New active end connection: " << rail->activeSwitchEnd << endl;
                }
            }
            else {
                std::cout << "Cannot switch track: rail is occupied by a train!" << endl;
            }
        }
	}

    if (key == GLFW_KEY_DELETE) {
        if (!railManager->IsBuildMode() && !trainEditor->IsTrainEditMode()) {
            if (selectedRailIndex != -1) {
				Rail* rail = railManager->GetRails()[selectedRailIndex];
                if (rail->deleteable && !railManager->CheckRailOccupied(rail)) {
                    railManager->RemoveRail(selectedRailIndex);
                    selectedRailIndex = -1;
                    selectedRailT = -1.0f;
                    changingTrackSwitch = false;
                }
                else {
					std::cout << "Cannot delete rail!" << endl;
                }
            }
            if (selectedTrainIndex != -1) {
				Train* train = railManager->GetTrains()[selectedTrainIndex];
                if (!train->IsBusy()) {
                    railManager->RemoveTrain(selectedTrainIndex);
                    selectedTrainIndex = -1;
                }
                else {
                    std::cout << "Cannot delete train: it is busy!" << endl;
                }
            }
		}
    }       
}


void TrainGame::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void TrainGame::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
	camera->OnMouseMove(window, mouseX, mouseY, deltaX, deltaY);
    if (railManager->IsBuildMode() && railManager->IsStartSelected()) {
        glm::mat4 viewMatrix = camera->GetViewMatrix();
        glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
        glm::ivec2 resolution = window->GetResolution();

        glm::vec3 rayDir = RayUtils::GetMouseRay(mouseX, mouseY, resolution.x, resolution.y, viewMatrix, projectionMatrix);
        glm::vec3 rayOrigin = camera->position;

		float groundDist = RayUtils::IntersectPlane(rayOrigin, rayDir, 0.0f);
        glm::vec3 intersectionPoint = rayOrigin + groundDist * rayDir;
        LimitPoint(intersectionPoint);
        if (railManager->IsGridSnapping()) {
            // Round to nearest integer
            intersectionPoint.x = round(intersectionPoint.x);
            intersectionPoint.y = round(intersectionPoint.y);
            intersectionPoint.z = round(intersectionPoint.z);
		}

		groundMouseIntersectionPoint = intersectionPoint;
        if (groundDist >= 0.0f) {
            railManager->UpdatePreviewRail(intersectionPoint);
		}
    }
}


void TrainGame::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (button == MOUSE_BUTTON_LEFT) {
        glm::mat4 viewMatrix = camera->GetViewMatrix();
        glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
        glm::ivec2 resolution = window->GetResolution();

        glm::vec3 rayDir = RayUtils::GetMouseRay(mouseX, mouseY, resolution.x, resolution.y, viewMatrix, projectionMatrix);
		glm::vec3 rayOrigin = camera->position;

        float groundDist = RayUtils::IntersectPlane(rayOrigin, rayDir, 0.0f);
        glm::vec3 intersectionPoint = rayOrigin + groundDist * rayDir;

		LimitPoint(intersectionPoint);

        if (railManager->IsGridSnapping()) {
            // Round to nearest integer
            intersectionPoint.x = round(intersectionPoint.x);
            intersectionPoint.y = round(intersectionPoint.y);
            intersectionPoint.z = round(intersectionPoint.z);
        }
        railManager->BuildRailUtil(intersectionPoint);

        trainEditor->Use(mouseX, resolution.y - mouseY);

        if (railManager->IsBuildMode() || trainEditor->IsTrainEditMode()) {
			return; // Ignore clicks in build mode or train edit mode
		}

		int trainIndex = railManager->GetTrainIndex(rayOrigin, rayDir);
		selectedTrainIndex = trainIndex;
        if (trainIndex != -1) {
            std::cout << "Clicked on train index: " << trainIndex << endl;
            return;
        }

        int railIndex = railManager->GetRailIndex(rayOrigin, rayDir);
        selectedRailIndex = railIndex;
        if (railIndex != -1) {
            std::cout << "Clicked on rail index: " << railIndex << endl;
            selectedRailT = railManager->GetRelativeT();

            if (trainEditor->IsReadyForDeployment()) {
                Train* newTrain = trainEditor->DeployTrain();
                newTrain->PlaceOnRail(railManager->GetRails()[railIndex], 0.5, true);
                if (newTrain) {
                    railManager->AddTrain(newTrain);
                }
            }
        }
        else {
            selectedRailT = -1.0f;
			changingTrackSwitch = false;
        }

        if (groundDist >= 0.0f) {
            std::cout << "Clicked on ground at: (" << intersectionPoint.x << ", " << intersectionPoint.y << ", " << intersectionPoint.z << ")" << endl;
        }
    }

    if (button == MOUSE_BUTTON_RIGHT) {
		// selectedRailIndex = -1;
	}
}


void TrainGame::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void TrainGame::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void TrainGame::OnWindowResize(int width, int height)
{
	trainEditor->OnResolutionChange(width, height);
	uiScale = height / 720.0f;
}
