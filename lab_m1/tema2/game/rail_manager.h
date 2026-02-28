#pragma once
#include "lab_m1/tema2/game/objects/rail.h"
#include "lab_m1/tema2/game/objects/train.h"
#include "lab_m1/tema2/game/objects/ground.h"
#include <vector>

struct SnapPoint {
	Rail* rail; // The existing rail object
	bool isEnd; // false = start (t = 0), tue = end (t = 1)
	glm::vec3 position;
	bool isValid;
	float t;
};

class RailManager {
public:
	RailManager() 
		: buildMode(false), curvedMode(false), startSelected(false),
		endSelected(false), previewRail(nullptr), angle(90.0f), railIndex(-1), relativeT(0.0f),
		snapRailIsEnd(false), snapRailStart(nullptr), gridSnapping(true)
	{
		rails = std::vector<Rail*>();
		debugSphere = object3D::CreateSphere("debugSphere", glm::vec3(0), 0.5f, glm::vec3(0, 1, 0), true);
		middleDebugSphere = object3D::CreateSphere("middleDebugSphere", glm::vec3(0), 0.5f, glm::vec3(1, 0, 0), true);
	}

	~RailManager() {
		for (auto rail : rails) {
			delete rail;
		}
	}

	void AddRail(Rail* rail);
	void RemoveRail(int index);
	void RemoveTrain(int index);
	std::pair<Rail*, Rail*> SplitRail(Rail* rail, float t);
	void RenderAllRails(Shader* shader, Shader* highlightShader, int selectedIndex, int trackSwitching, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	int GetRailIndex(glm::vec3 rayOrigin, glm::vec3 rayDirection);
	int GetTrainIndex(glm::vec3 rayOrigin, glm::vec3 rayDirection);
	Rail* CreateRail(glm::vec3 start, glm::vec3 end, float sweepAngleDeg, float width = 1.0f);
	Rail* CreateRail();
	
	void BuildRailUtil(glm::vec3 &intersectionPoint);

	void UpdatePreviewRail(glm::vec3 intersectionPoint);

	bool IsBuildMode() { return buildMode; };
	void ToggleBuildMode();
	bool IsStartSelected() { return startSelected; };
	bool IsEndSelected() { return endSelected; };
	bool IsCurvedMode() { return curvedMode; };
	bool IsGridSnapping() { return gridSnapping; };
	void ToggleCurvedMode();

	glm::vec3 GetStartPos() { return startPos; };
	glm::vec3 GetEndPos() { return endPos; };

	void SetBuildMode(bool mode) { this->buildMode = mode; };
	void SetCurvedMode(bool mode) { this->curvedMode = mode; };
	void SetStartSelected(bool selected) { this->startSelected = selected; };
	void SetEndSelected(bool selected) { this->endSelected = selected; };
	void SetStartPos(const glm::vec3& pos) { this->startPos = pos; };
	void SetEndPos(const glm::vec3& pos) { this->endPos = pos; };
	void SetGridSnapping(bool snapping) { this->gridSnapping = snapping; };

	void IncrementAngle(float delta = 15.0f);
	void DecrementAngle(float delta = 15.0f);

	SnapPoint GetClosestSnapPoint(glm::vec3 position, float threshold = 2.0f);
	std::vector<SnapPoint> GetSnapPoints(glm::vec3 position, float threshold = 2.0f);
	SnapPoint GetMiddleSnapPoint(glm::vec3 position, float threshold = 2.0f);

	void Connect(Rail* newRail, bool newRailEndIs1, Rail* targetRail, bool targetRailEndIs1);
	void RenderDebug(Shader* shader, const glm::mat4& view, const glm::mat4& proj);
	std::vector<Rail*>& GetRails() { return rails; };
	float GetRelativeT() { return relativeT; };

	bool CheckTrainsOnRail(Rail* rail);
	void AddTrain(Train* train) { trains.push_back(train); };
	void ComputeCosmeticsForStraightRail(StraightRail* rail);
	void ComputeCosmeticsForCurvedRail(CurvedRail* rail);
	void ComputeCosmeticsForRail(Rail* rail);

	void SetGroundTiles(const std::vector<Ground*>& tiles) { groundTiles = tiles; };

	bool CheckRailOccupied(Rail* rail);

	std::vector<Train*> GetTrains() { return trains; };

private:
	std::vector<Train*> trains;

	std::vector<Rail*> rails;
	bool buildMode;
	bool curvedMode;

	bool startSelected;
	glm::vec3 startPos;

	bool endSelected;
	glm::vec3 endPos;

	Rail* previewRail;
	
	float angle;

	int railIndex;
	float relativeT;
	float unit = 4.0f;

	Mesh* debugSphere;
	Mesh* middleDebugSphere;

	bool snapRailIsEnd;
	Rail* snapRailStart;
	float snapRailT;

	bool gridSnapping;

	std::vector<Ground*> groundTiles;

	int GetGroundTileTypeAtPosition(glm::vec3 position);
};
