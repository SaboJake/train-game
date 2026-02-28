#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "utils/gl_utils.h"
#include "lab_m1/tema2/my_utils/object3D.h"

class Ground {
public:
	Ground(glm::vec3 position, int tileType)
		: pos(position), type(tileType) {
		mesh = GetMesh(type);
	}

	void Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	int GetTileType() { return type; }

	bool IsPositionInside(glm::vec3 position) {
		int halfTile = tileSize / 2;
		return (position.x >= pos.x - halfTile && position.x < pos.x + halfTile &&
				position.z >= pos.z - halfTile && position.z < pos.z + halfTile);
	}
private:
	glm::vec3 pos;
	float tileSize = 4; // should be constant for all tiles
	int type; // 0 = grass, 1 = water, 2 = hill
	Mesh* mesh;

	Mesh* GetMesh(int type);
};
