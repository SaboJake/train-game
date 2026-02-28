#include "object3D.h"

#include "core/engine.h"
#include "utils/gl_utils.h"

#include <glm/gtx/vector_angle.hpp>
#include <vector>

Mesh* object3D::CreateQuad(
	const std::string& name,
	glm::vec3 center,
	float length,
	float width,
	glm::vec3 color,
	bool fill)
{
	float halfLength = length / 2.0f;
	float halfWidth = width / 2.0f;
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(center.x - halfLength, center.y, center.z - halfWidth), color),
		VertexFormat(glm::vec3(center.x + halfLength, center.y, center.z - halfWidth), color),
		VertexFormat(glm::vec3(center.x + halfLength, center.y, center.z + halfWidth), color),
		VertexFormat(glm::vec3(center.x - halfLength, center.y, center.z + halfWidth), color),
	};
	Mesh* quad = new Mesh(name);
	std::vector<unsigned int> indices = {
		0, 1, 2,
		2, 3, 0
	};
	if (!fill) {
		quad->SetDrawMode(GL_LINE);
	}
	quad->InitFromData(vertices, indices);
	return quad;
}

Mesh* object3D::CreateTriangle(
	const std::string& name,
	glm::vec3 center,
	float sideLength,
	glm::vec3 color,
	bool fill)
{
	float halfSide = sideLength / 2.0f;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(center.x, center.y, center.z + halfSide), color),
		VertexFormat(glm::vec3(center.x + halfSide, center.y, center.z - halfSide), color),
		VertexFormat(glm::vec3(center.x - halfSide, center.y, center.z - halfSide), color),
	};

	Mesh* triangle = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2 };

	if (!fill) {
		triangle->SetDrawMode(GL_LINE);
	}
	triangle->InitFromData(vertices, indices);
	return triangle;
}

Mesh* object3D::CreateRectangle(const std::string& name,
	glm::vec3 center,
	float length,
	float width,
	glm::vec3 color,
	bool fill)
{
	float height = length;
	float halfWidth = width / 2.0f;
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(center.x - halfWidth, center.y - height / 2.0f, center.z), color),
		VertexFormat(glm::vec3(center.x + halfWidth, center.y - height / 2.0f, center.z), color),
		VertexFormat(glm::vec3(center.x + halfWidth, center.y + height / 2.0f, center.z), color),
		VertexFormat(glm::vec3(center.x - halfWidth, center.y + height / 2.0f, center.z), color),
	};

	std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

	Mesh* rectangle = new Mesh(name);
	if (!fill) {
		rectangle->SetDrawMode(GL_LINE);
	}
	rectangle->InitFromData(vertices, indices);
	return rectangle;
}

Mesh* object3D::CreateCube(
	const std::string& name,
	glm::vec3 center,
	float sideLength,
	glm::vec3 color,
	bool fill)
{
	float halfSide = sideLength / 2.0f;
	std::vector<VertexFormat> vertices =
	{
		// Front face
		VertexFormat(glm::vec3(center.x - halfSide, center.y - halfSide, center.z + halfSide), color),
		VertexFormat(glm::vec3(center.x + halfSide, center.y - halfSide, center.z + halfSide), color),
		VertexFormat(glm::vec3(center.x + halfSide, center.y + halfSide, center.z + halfSide), color),
		VertexFormat(glm::vec3(center.x - halfSide, center.y + halfSide, center.z + halfSide), color),
		// Back face
		VertexFormat(glm::vec3(center.x - halfSide, center.y - halfSide, center.z - halfSide), color),
		VertexFormat(glm::vec3(center.x + halfSide, center.y - halfSide, center.z - halfSide), color),
		VertexFormat(glm::vec3(center.x + halfSide, center.y + halfSide, center.z - halfSide), color),
		VertexFormat(glm::vec3(center.x - halfSide, center.y + halfSide, center.z - halfSide), color),
	};
	Mesh* cube = new Mesh(name);
	std::vector<unsigned int> indices = {
		// Front face
		0, 1, 2, 2, 3, 0,
		// Right face
		1, 5, 6, 6, 2, 1,
		// Back face
		5, 4, 7, 7, 6, 5,
		// Left face
		4, 0, 3, 3, 7, 4,
		// Top face
		3, 2, 6, 6, 7, 3,
		// Bottom face
		4, 5, 1, 1, 0, 4
	};
	if (!fill) {
		cube->SetDrawMode(GL_LINE);
	}
	cube->InitFromData(vertices, indices);
	return cube;
}

Mesh* object3D::CreatePyramid(
	const std::string& name,
	glm::vec3 center,
	float baseSideLength,
	float height,
	glm::vec3 color,
	bool fill)
{
	float halfSide = baseSideLength / 2.0f;

	glm::vec3 apex = center + glm::vec3(0, height, 0);

	std::vector<VertexFormat> vertices =
	{

		VertexFormat(center + glm::vec3(-halfSide, 0, halfSide), color),  // 0: Front-Left
		VertexFormat(center + glm::vec3(halfSide, 0, halfSide), color),   // 1: Front-Right
		VertexFormat(center + glm::vec3(halfSide, 0, -halfSide), color),  // 2: Back-Right
		VertexFormat(center + glm::vec3(-halfSide, 0, -halfSide), color), // 3: Back-Left

		VertexFormat(apex, color) // 4
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,
		2, 3, 0,
		0, 4, 1, // Front
		1, 4, 2, // Right
		2, 4, 3, // Back
		3, 4, 0  // Left
	};

	Mesh* pyramid = new Mesh(name);
	if (!fill) {
		pyramid->SetDrawMode(GL_LINE);
	}
	pyramid->InitFromData(vertices, indices);
	return pyramid;
}

Mesh* object3D::CreateSphere(
	const std::string& name,
	glm::vec3 center,
	float radius,
	glm::vec3 color,
	bool fill,
	int stacks,
	int slices)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;
	for (int stack = 0; stack <= stacks; stack++) {
		float phi = glm::pi<float>() * stack / stacks;
		for (int slice = 0; slice <= slices; slice++) {
			float theta = 2.0f * glm::pi<float>() * slice / slices;
			float x = center.x + radius * sin(phi) * cos(theta);
			float y = center.y + radius * cos(phi);
			float z = center.z + radius * sin(phi) * sin(theta);
			vertices.push_back(VertexFormat(glm::vec3(x, y, z), color));
		}
	}
	for (int stack = 0; stack < stacks; stack++) {
		for (int slice = 0; slice < slices; slice++) {
			int first = (stack * (slices + 1)) + slice;
			int second = first + slices + 1;
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);
			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}
	Mesh* sphere = new Mesh(name);
	if (!fill) {
		sphere->SetDrawMode(GL_LINE);
	}
	sphere->InitFromData(vertices, indices);
	return sphere;
}

Mesh* object3D::CreateCylinder(
	const std::string& name,
	glm::vec3 center,
	float radius,
	float height,
	glm::vec3 color,
	bool fill,
	int slices)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;
	float halfHeight = height / 2.0f;

	for (int i = 0; i <= slices; i++) {
		float theta = 2.0f * glm::pi<float>() * i / slices;
		float x = center.x + radius * cos(theta);
		float z = center.z + radius * sin(theta);
		vertices.push_back(VertexFormat(glm::vec3(x, center.y - halfHeight, z), color)); // Bottom circle
		vertices.push_back(VertexFormat(glm::vec3(x, center.y + halfHeight, z), color)); // Top circle
	}

	int bottomCenterIndex = vertices.size();
	vertices.push_back(VertexFormat(glm::vec3(center.x, center.y - halfHeight, center.z), color)); // Bottom center
	int topCenterIndex = vertices.size();
	vertices.push_back(VertexFormat(glm::vec3(center.x, center.y + halfHeight, center.z), color)); // Top center

	for (int i = 0; i < slices; i++) {
		int bottom1 = i * 2;
		int top1 = bottom1 + 1;
		int bottom2 = ((i + 1) % slices) * 2;
		int top2 = bottom2 + 1;

		indices.push_back(bottom1);
		indices.push_back(top1);
		indices.push_back(bottom2);
		indices.push_back(top1);
		indices.push_back(top2);
		indices.push_back(bottom2);
	}

	// Generate indices for the bottom circle
	for (int i = 0; i < slices; i++) {
		indices.push_back(bottomCenterIndex);
		indices.push_back(i * 2);
		indices.push_back(((i + 1) % slices) * 2);
	}

	// Generate indices for the top circle
	for (int i = 0; i < slices; i++) {
		indices.push_back(topCenterIndex);
		indices.push_back(((i + 1) % slices) * 2 + 1);
		indices.push_back(i * 2 + 1);
	}

	Mesh* cylinder = new Mesh(name);
	if (!fill) {
		cylinder->SetDrawMode(GL_LINE);
	}
	cylinder->InitFromData(vertices, indices);
	return cylinder;
}

Mesh* object3D::CreateCone(
	const std::string& name,
	glm::vec3 center,
	float radius,
	float height,
	glm::vec3 color,
	bool fill,
	int slices)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;
	float halfHeight = height / 2.0f;
	glm::vec3 apex = glm::vec3(center.x, center.y + halfHeight, center.z);

	// Generate base vertices
	for (int i = 0; i < slices; i++) {
		float theta = 2.0f * glm::pi<float>() * i / slices;
		float x = center.x + radius * cos(theta);
		float z = center.z + radius * sin(theta);
		vertices.push_back(VertexFormat(glm::vec3(x, center.y - halfHeight, z), color)); // Base circle
	}

	// Add center vertex for the base
	int baseCenterIndex = vertices.size();
	vertices.push_back(VertexFormat(glm::vec3(center.x, center.y - halfHeight, center.z), color)); // Base center

	// Add apex vertex
	int apexIndex = vertices.size();
	vertices.push_back(VertexFormat(apex, color)); // Apex vertex

	// Generate indices for the sides
	for (int i = 0; i < slices; i++) {
		int base1 = i;
		int base2 = (i + 1) % slices;
		indices.push_back(base1);
		indices.push_back(base2);
		indices.push_back(apexIndex);
	}

	// Generate indices for the base circle
	for (int i = 0; i < slices; i++) {
		int base1 = i;
		int base2 = (i + 1) % slices;
		indices.push_back(baseCenterIndex);
		indices.push_back(base1);
		indices.push_back(base2);
	}

	Mesh* cone = new Mesh(name);
	if (!fill) {
		cone->SetDrawMode(GL_LINE);
	}
	cone->InitFromData(vertices, indices);
	return cone;
}

Mesh* object3D::CreateHalfCylinder(
	const std::string& name,
	glm::vec3 center,
	float radius,
	float height,
	glm::vec3 color,
	bool fill,
	int slices)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;
	float halfHeight = height / 2.0f;

	// Generate vertices for the half-cylinder
	for (int i = 0; i <= slices; i++) {
		float theta = glm::pi<float>() * i / slices; // Half-circle
		float x = center.x + radius * cos(theta);
		float z = center.z + radius * sin(theta);
		vertices.push_back(VertexFormat(glm::vec3(x, center.y - halfHeight, z), color)); // Bottom edge
		vertices.push_back(VertexFormat(glm::vec3(x, center.y + halfHeight, z), color)); // Top edge
	}

	// Add center vertices for the bottom and top edges
	int bottomCenterIndex = vertices.size();
	vertices.push_back(VertexFormat(glm::vec3(center.x, center.y - halfHeight, center.z), color)); // Bottom center
	int topCenterIndex = vertices.size();
	vertices.push_back(VertexFormat(glm::vec3(center.x, center.y + halfHeight, center.z), color)); // Top center

	// Generate indices for the sides
	for (int i = 0; i < slices; i++) {
		int bottom1 = i * 2;
		int top1 = bottom1 + 1;
		int bottom2 = ((i + 1) % (slices + 1)) * 2;
		int top2 = bottom2 + 1;

		indices.push_back(bottom1);
		indices.push_back(top1);
		indices.push_back(bottom2);
		indices.push_back(top1);
		indices.push_back(top2);
		indices.push_back(bottom2);
	}

	// Generate indices for the bottom face
	for (int i = 0; i < slices; i++) {
		indices.push_back(bottomCenterIndex);
		indices.push_back(i * 2);
		indices.push_back(((i + 1) % (slices + 1)) * 2);
	}

	// Generate indices for the top face
	for (int i = 0; i < slices; i++) {
		indices.push_back(topCenterIndex);
		indices.push_back(((i + 1) % (slices + 1)) * 2 + 1);
		indices.push_back(i * 2 + 1);
	}

	Mesh* halfCylinder = new Mesh(name);
	if (!fill) {
		halfCylinder->SetDrawMode(GL_LINE);
	}
	halfCylinder->InitFromData(vertices, indices);
	return halfCylinder;
}

Mesh* object3D::CreateCurvedRail(
	const std::string& name,
	glm::vec3 center,
	float radius,
	float arcAngleDeg,
	float width,
	float height,
	glm::vec3 color,
	int slices)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	float arcAngleRad = glm::radians(arcAngleDeg);

	for (int i = 0; i <= slices; i++) {
		float t = (float)i / slices;
		float theta = t * arcAngleRad;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		float innerRadius = radius - halfWidth;
		float outerRadius = radius + halfWidth;

		vertices.push_back(VertexFormat(glm::vec3(center.x + innerRadius * cosTheta,
			center.y - halfHeight, center.z + innerRadius * sinTheta), color));

		vertices.push_back(VertexFormat(glm::vec3(center.x + outerRadius * cosTheta,
			center.y - halfHeight, center.z + outerRadius * sinTheta), color));

		vertices.push_back(VertexFormat(glm::vec3(center.x + outerRadius * cosTheta,
			center.y + halfHeight, center.z + outerRadius * sinTheta), color));

		vertices.push_back(VertexFormat(glm::vec3(center.x + innerRadius * cosTheta,
			center.y + halfHeight, center.z + innerRadius * sinTheta), color));
	}

	for (int i = 0; i < slices; i++) {
		int start = 4 * i;

		// Top Face (3 -> 2 -> 6 -> 7)
		indices.push_back(start + 3); indices.push_back(start + 2); indices.push_back(start + 6);
		indices.push_back(start + 6); indices.push_back(start + 7); indices.push_back(start + 3);

		// Bottom Face (0 -> 4 -> 5 -> 1)
		indices.push_back(start + 0); indices.push_back(start + 4); indices.push_back(start + 5);
		indices.push_back(start + 5); indices.push_back(start + 1); indices.push_back(start + 0);

		// Outer Face (1 -> 5 -> 6 -> 2)
		indices.push_back(start + 1); indices.push_back(start + 5); indices.push_back(start + 6);
		indices.push_back(start + 6); indices.push_back(start + 2); indices.push_back(start + 1);

		// Inner Face (0 -> 3 -> 7 -> 4)
		indices.push_back(start + 0); indices.push_back(start + 3); indices.push_back(start + 7);
		indices.push_back(start + 7); indices.push_back(start + 4); indices.push_back(start + 0);
	}

	Mesh* rail = new Mesh(name);
	rail->InitFromData(vertices, indices);
	return rail;
}

Mesh* object3D::CreateCurvedRail(
	const std::string& name,
	glm::vec3 center,
	float radius,
	float startAngleDeg,
	float arcAngleDeg,
	float width,
	float height,
	glm::vec3 color,
	int slices)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	float startRad = glm::radians(startAngleDeg);
	float arcAngleRad = glm::radians(arcAngleDeg);

	for (int i = 0; i <= slices; i++) {
		float t = (float)i / slices;

		float theta = startRad + t * arcAngleRad;

		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		float innerRadius = radius - halfWidth;
		float outerRadius = radius + halfWidth;

		vertices.push_back(VertexFormat(glm::vec3(center.x + innerRadius * cosTheta,
			center.y - halfHeight, center.z + innerRadius * sinTheta), color));

		vertices.push_back(VertexFormat(glm::vec3(center.x + outerRadius * cosTheta,
			center.y - halfHeight, center.z + outerRadius * sinTheta), color));

		vertices.push_back(VertexFormat(glm::vec3(center.x + outerRadius * cosTheta,
			center.y + halfHeight, center.z + outerRadius * sinTheta), color));

		vertices.push_back(VertexFormat(glm::vec3(center.x + innerRadius * cosTheta,
			center.y + halfHeight, center.z + innerRadius * sinTheta), color));
	}

	for (int i = 0; i < slices; i++) {
		int start = 4 * i;

		// Top Face (3 -> 2 -> 6 -> 7)
		indices.push_back(start + 3); indices.push_back(start + 2); indices.push_back(start + 6);
		indices.push_back(start + 6); indices.push_back(start + 7); indices.push_back(start + 3);

		// Bottom Face (0 -> 4 -> 5 -> 1)
		indices.push_back(start + 0); indices.push_back(start + 4); indices.push_back(start + 5);
		indices.push_back(start + 5); indices.push_back(start + 1); indices.push_back(start + 0);

		// Outer Face (1 -> 5 -> 6 -> 2)
		indices.push_back(start + 1); indices.push_back(start + 5); indices.push_back(start + 6);
		indices.push_back(start + 6); indices.push_back(start + 2); indices.push_back(start + 1);

		// Inner Face (0 -> 3 -> 7 -> 4)
		indices.push_back(start + 0); indices.push_back(start + 3); indices.push_back(start + 7);
		indices.push_back(start + 7); indices.push_back(start + 4); indices.push_back(start + 0);
	}

	Mesh* rail = new Mesh(name);
	rail->InitFromData(vertices, indices);
	return rail;
}

Mesh* object3D::CreateHollowHalfCylinder(
	const std::string& name,
	glm::vec3 center,
	float outerRadius,
	float innerRadius,
	float height,
	glm::vec3 color,
	bool fill,
	int slices)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;
	float halfHeight = height / 2.0f;

	// Generate vertices for the hollow half-cylinder
	for (int i = 0; i <= slices; i++) {
		float theta = glm::pi<float>() * i / slices; // Half-circle
		float outerX = center.x + outerRadius * cos(theta);
		float outerZ = center.z + outerRadius * sin(theta);
		float innerX = center.x + innerRadius * cos(theta);
		float innerZ = center.z + innerRadius * sin(theta);

		// Outer edge
		vertices.push_back(VertexFormat(glm::vec3(outerX, center.y - halfHeight, outerZ), color)); // Bottom outer
		vertices.push_back(VertexFormat(glm::vec3(outerX, center.y + halfHeight, outerZ), color)); // Top outer

		// Inner edge
		vertices.push_back(VertexFormat(glm::vec3(innerX, center.y - halfHeight, innerZ), color)); // Bottom inner
		vertices.push_back(VertexFormat(glm::vec3(innerX, center.y + halfHeight, innerZ), color)); // Top inner
	}

	// Generate indices for the sides
	for (int i = 0; i < slices; i++) {
		int outerBottom1 = i * 4;
		int outerTop1 = outerBottom1 + 1;
		int outerBottom2 = outerBottom1 + 4;
		int outerTop2 = outerBottom2 + 1;

		int innerBottom1 = outerBottom1 + 2;
		int innerTop1 = innerBottom1 + 1;
		int innerBottom2 = outerBottom2 + 2;
		int innerTop2 = innerBottom2 + 1;

		// Outer face
		indices.push_back(outerBottom1);
		indices.push_back(outerTop1);
		indices.push_back(outerBottom2);
		indices.push_back(outerTop1);
		indices.push_back(outerTop2);
		indices.push_back(outerBottom2);

		// Inner face
		indices.push_back(innerBottom2);
		indices.push_back(innerTop1);
		indices.push_back(innerBottom1);
		indices.push_back(innerBottom2);
		indices.push_back(innerTop2);
		indices.push_back(innerTop1);

		// Top face
		indices.push_back(outerTop1);
		indices.push_back(innerTop1);
		indices.push_back(outerTop2);
		indices.push_back(innerTop1);
		indices.push_back(innerTop2);
		indices.push_back(outerTop2);

		// Bottom face
		indices.push_back(innerBottom1);
		indices.push_back(outerBottom1);
		indices.push_back(innerBottom2);
		indices.push_back(outerBottom1);
		indices.push_back(outerBottom2);
		indices.push_back(innerBottom2);
	}

	Mesh* hollowHalfCylinder = new Mesh(name);
	if (!fill) {
		hollowHalfCylinder->SetDrawMode(GL_LINE);
	}
	hollowHalfCylinder->InitFromData(vertices, indices);
	return hollowHalfCylinder;
}

Mesh* object3D::CreateCurvedTunnel(
	const std::string& name,
	glm::vec3 center,
	float trackRadius,
	float tubeOuterRadius,
	float tubeInnerRadius,
	float arcAngleDeg,
	glm::vec3 color,
	int slices,
	int stacks)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	float arcAngleRad = glm::radians(arcAngleDeg);

	// Generate grid of vertices
	for (int i = 0; i <= slices; i++) {
		float t = (float)i / slices;
		float theta = t * arcAngleRad;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		for (int j = 0; j <= stacks; j++) {
			float phi = glm::pi<float>() * (float)j / stacks;
			float cosPhi = cos(phi);
			float sinPhi = sin(phi);

			glm::vec3 trackPos = glm::vec3(trackRadius * cosTheta, 0, trackRadius * sinTheta);
			glm::vec3 radial = glm::vec3(cosTheta, 0, sinTheta);
			glm::vec3 up = glm::vec3(0, 1, 0);

			glm::vec3 outerPos = center + trackPos + radial * (tubeOuterRadius * cosPhi) + up * (tubeOuterRadius * sinPhi);
			vertices.push_back(VertexFormat(outerPos, color));

			glm::vec3 innerPos = center + trackPos + radial * (tubeInnerRadius * cosPhi) + up * (tubeInnerRadius * sinPhi);
			vertices.push_back(VertexFormat(innerPos, color));
		}
	}

	// Generate indices for the tunnel walls
	for (int i = 0; i < slices; i++) {
		for (int j = 0; j < stacks; j++) {
			int currentSlice = i * (stacks + 1);
			int nextSlice = (i + 1) * (stacks + 1);

			int k = (currentSlice + j) * 2;
			int kNext = (nextSlice + j) * 2;

			int outerBL = k;
			int outerBR = kNext;
			int outerTR = kNext + 2;
			int outerTL = k + 2;

			int innerBL = k + 1;
			int innerBR = kNext + 1;
			int innerTR = kNext + 3;
			int innerTL = k + 3;

			// Outer Shell
			indices.push_back(outerBL); indices.push_back(outerBR); indices.push_back(outerTR);
			indices.push_back(outerTR); indices.push_back(outerTL); indices.push_back(outerBL);

			// Inner Shell
			indices.push_back(innerBL); indices.push_back(innerTL); indices.push_back(innerTR);
			indices.push_back(innerTR); indices.push_back(innerBR); indices.push_back(innerBL);
		}
	}

	// Add front and back sides of the tunnel
	for (int j = 0; j <= stacks; j++) {
		int frontOuter = j * 2;
		int frontInner = j * 2 + 1;
		int backOuter = (slices * (stacks + 1) + j) * 2;
		int backInner = backOuter + 1;

		if (j < stacks) {
			// Front cap
			indices.push_back(frontOuter);
			indices.push_back(frontInner);
			indices.push_back(frontInner + 2);

			indices.push_back(frontInner + 2);
			indices.push_back(frontOuter + 2);
			indices.push_back(frontOuter);

			// Back cap
			indices.push_back(backOuter);
			indices.push_back(backOuter + 2);
			indices.push_back(backInner + 2);

			indices.push_back(backInner + 2);
			indices.push_back(backInner);
			indices.push_back(backOuter);
		}
	}

	Mesh* tunnel = new Mesh(name);
	tunnel->InitFromData(vertices, indices);
	return tunnel;
}

Mesh* object3D::CreateCurvedTunnel(
	const std::string& name,
	glm::vec3 center,
	float trackRadius,
	float tubeOuterRadius,
	float tubeInnerRadius,
	float startAngleDeg,
	float arcAngleDeg,
	glm::vec3 color,
	int slices,
	int stacks)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	float startRad = glm::radians(startAngleDeg);
	float arcAngleRad = glm::radians(arcAngleDeg);

	for (int i = 0; i <= slices; i++) {
		float t = (float)i / slices;
		float theta = startRad + t * arcAngleRad;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		for (int j = 0; j <= stacks; j++) {
			float phi = glm::pi<float>() * (float)j / stacks;
			float cosPhi = cos(phi);
			float sinPhi = sin(phi);

			glm::vec3 trackPos = glm::vec3(trackRadius * cosTheta, 0, trackRadius * sinTheta);
			glm::vec3 radial = glm::vec3(cosTheta, 0, sinTheta);
			glm::vec3 up = glm::vec3(0, 1, 0);

			glm::vec3 outerPos = center + trackPos + radial * (tubeOuterRadius * cosPhi) + up * (tubeOuterRadius * sinPhi);
			vertices.push_back(VertexFormat(outerPos, color));

			glm::vec3 innerPos = center + trackPos + radial * (tubeInnerRadius * cosPhi) + up * (tubeInnerRadius * sinPhi);
			vertices.push_back(VertexFormat(innerPos, color));
		}
	}

	for (int i = 0; i < slices; i++) {
		for (int j = 0; j < stacks; j++) {
			int currentSlice = i * (stacks + 1);
			int nextSlice = (i + 1) * (stacks + 1);

			int k = (currentSlice + j) * 2;
			int kNext = (nextSlice + j) * 2;

			int outerBL = k;
			int outerBR = kNext;
			int outerTR = kNext + 2;
			int outerTL = k + 2;

			int innerBL = k + 1;
			int innerBR = kNext + 1;
			int innerTR = kNext + 3;
			int innerTL = k + 3;

			// Outer Shell
			indices.push_back(outerBL); indices.push_back(outerBR); indices.push_back(outerTR);
			indices.push_back(outerTR); indices.push_back(outerTL); indices.push_back(outerBL);

			// Inner Shell
			indices.push_back(innerBL); indices.push_back(innerTL); indices.push_back(innerTR);
			indices.push_back(innerTR); indices.push_back(innerBR); indices.push_back(innerBL);
		}
	}

	// Add front and back sides of the tunnel
	for (int j = 0; j <= stacks; j++) {
		int frontOuter = j * 2;
		int frontInner = j * 2 + 1;
		int backOuter = (slices * (stacks + 1) + j) * 2;
		int backInner = backOuter + 1;

		if (j < stacks) {
			// Front cap
			indices.push_back(frontOuter);
			indices.push_back(frontInner);
			indices.push_back(frontInner + 2);

			indices.push_back(frontInner + 2);
			indices.push_back(frontOuter + 2);
			indices.push_back(frontOuter);

			// Back cap
			indices.push_back(backOuter);
			indices.push_back(backOuter + 2);
			indices.push_back(backInner + 2);

			indices.push_back(backInner + 2);
			indices.push_back(backInner);
			indices.push_back(backOuter);
		}
	}

	Mesh* tunnel = new Mesh(name);
	tunnel->InitFromData(vertices, indices);
	return tunnel;
}
