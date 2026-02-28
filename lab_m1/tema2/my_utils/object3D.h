#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

namespace object3D
{
	Mesh* CreateQuad(const std::string& name, glm::vec3 center, float length, float width, glm::vec3 color, bool fill = true);
	Mesh* CreateTriangle(const std::string& name, glm::vec3 center, float sideLength, glm::vec3 color, bool fill = true);
	Mesh* CreateRectangle(const std::string& name, glm::vec3 center, float length, float width, glm::vec3 color, bool fill);
	Mesh* CreateCube(const std::string& name, glm::vec3 center, float sideLength, glm::vec3 color, bool fill = true);
	Mesh* CreatePyramid(const std::string& name, glm::vec3 center, float baseSideLength, float height, glm::vec3 color, bool fill = true);
	Mesh* CreateSphere(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill = true, int stacks = 20, int slices = 20);
	Mesh* CreateCylinder(const std::string& name, glm::vec3 center, float radius, float height, glm::vec3 color, bool fill = true, int slices = 20);
	Mesh* CreateCone(const std::string& name, glm::vec3 center, float radius, float height, glm::vec3 color, bool fill = true, int slices = 20);
	Mesh* CreateHalfCylinder(const std::string& name, glm::vec3 center, float radius, float height, glm::vec3 color, bool fill = true, int slices = 20);
	Mesh* CreateHollowHalfCylinder(const std::string& name, glm::vec3 center, float outerRadius, float innerRadius, float height, glm::vec3 color, bool fill = true, int slices = 20);

	Mesh* CreateCurvedRail(const std::string& name, glm::vec3 center, float radius, float arcAngleDeg, float width, float height, glm::vec3 color, int slices = 20);
	Mesh* CreateCurvedRail(const std::string& name, glm::vec3 center, float radius, float startAngleDeg, float arcAngleDeg, float width, float height, glm::vec3 color, int slices = 20);
	Mesh* CreateCurvedTunnel(const std::string& name, glm::vec3 center, float trackRadius, float tubeOuterRadius, float tubeInnerRadius, float arcAngleDeg, glm::vec3 color, int slices = 20, int stacks = 10);
	Mesh* CreateCurvedTunnel(const std::string& name, glm::vec3 center, float trackRadius, float tubeOuterRadius, float tubeInnerRadius, float startAngleDeg, float arcAngleDeg, glm::vec3 color, int slices = 20, int stacks = 10);
}
