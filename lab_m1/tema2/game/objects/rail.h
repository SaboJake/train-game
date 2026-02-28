#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/gpu/mesh.h"
#include "utils/gl_utils.h"
#include "core/gpu/shader.h"
#include "lab_m1/tema2/my_utils/object3D.h"

class Rail {
public:
    virtual ~Rail() {
    }

    // Connections
    std::vector<Rail*> connectedStart;
	std::vector<Rail*> connectedEnd;
    int activeSwitchStart = 0;
	int activeSwitchEnd = 0;
	int type = 0; // 0 = straight, 1 = curved
	bool deleteable = true; // platform rails should not be deleteable

    void ConnectTo(Rail* other, bool myEnd, bool otherEnd) {
        if (myEnd) this->connectedEnd.push_back(other);
		else this->connectedStart.push_back(other);

		if (otherEnd) other->connectedEnd.push_back(this);
		else other->connectedStart.push_back(this);
    }

    virtual glm::vec3 GetPos(float t) = 0;
    virtual glm::vec3 GetForwardVector(float t) = 0;
    virtual float GetLength() = 0;

    virtual void Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) = 0;
    virtual void RenderMinimap(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) = 0;
};

struct GroundUtil {
	int tileType; // 0 = grass, 1 = water, 2 = hill
	glm::vec3 translation;
    float scale;
};

class StraightRail : public Rail {
public:
    StraightRail(glm::vec3 start, glm::vec3 end, float width = 1.0f, int constructionType = 0);
    //~StraightRail();

    glm::vec3 GetPos(float t) override;
    glm::vec3 GetForwardVector(float t) override;
    float GetLength() override;
    void Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;
	void RenderMinimap(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;
	void AddGroundUtil(const GroundUtil& gu) { groundUtils.push_back(gu); }
	void ClearGroundUtils() { groundUtils.clear(); }
private:
    glm::vec3 startPosition;
    glm::vec3 endPosition;
    float trackWidth;

    // Meshes
    Mesh* meshRailLeft;
    Mesh* meshRailRight;
	Mesh* meshRailMM;
    Mesh* meshSleeper;

	Mesh* bridgeMesh;
	Mesh* tunnelMesh;
	int constructionType; // 0 = normal, 1 = bridge, 2 = tunnel

	std::vector<GroundUtil> groundUtils;

    // Cache for sleeper transforms to avoid recalculating every frame
    std::vector<glm::mat4> sleeperTransforms;

    void RenderCosmetics(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
};

struct GroundUtilMesh {
    int tileType; // 0 = grass, 1 = water, 2 = hill
    Mesh* mesh;
};

class CurvedRail : public Rail {
public:
    CurvedRail(glm::vec3 center, float radius, float startAngleDeg, float sweepAngleDeg, float width = 1.0f, int constructionT = 0);
    //~CurvedRail();

    glm::vec3 GetPos(float t) override;
    glm::vec3 GetForwardVector(float t) override;
    float GetLength() override;
    void Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;
	void RenderMinimap(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;
	float GetSweepAngleDeg() { return glm::degrees(sweepAngleRad); }
	float GetStartAngleDeg() { return glm::degrees(startAngleRad); }
	float GetTrackWidth() { return trackWidth; }
	float GetRadius() { return radius; }
	void AddCosmeticMesh(GroundUtilMesh mesh) { cosmeticMeshes.push_back(mesh); }
    void ClearCosmeticMeshes() { cosmeticMeshes.clear(); }

private:
    glm::vec3 centerPosition;
    float radius;
    float startAngleRad;
    float endAngleRad;
    float sweepAngleRad;
    float direction; // 1 for CCW, -1 for CW
    float trackWidth;

    Mesh* meshRailInner;
    Mesh* meshRailOuter;
    Mesh* meshSleeper;
    Mesh* meshRailMM;

	std::vector<GroundUtilMesh> cosmeticMeshes;

    Mesh* bridgeMesh;
    Mesh* tunnelMesh;
    int constructionType; // 0 = normal, 1 = bridge, 2 = tunnel

    std::vector<glm::mat4> sleeperTransforms;

    Mesh* CreateBentRailMesh(std::string name, float radius, float width, float height, float startAngle, float endAngle, glm::vec3 color);
	void RenderCosmetics(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
};

CurvedRail* CreateCurvedRailFromPoints(glm::vec3 start, glm::vec3 end, float sweepAngleDeg, float width = 1.0f);
