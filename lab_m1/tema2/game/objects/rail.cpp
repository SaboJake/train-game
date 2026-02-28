#include "rail.h"
#include "lab_m1/tema2/my_utils/common_meshes.h"
#include <iostream>

StraightRail::StraightRail(glm::vec3 start, glm::vec3 end, float width, int cType)
	: startPosition(start), endPosition(end), trackWidth(width), constructionType(cType)
{
	float length = glm::distance(start, end);
	glm::vec3 direction = glm::normalize(end - start);
	glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0, 1, 0)));

	meshRailLeft = commonMeshes::getMesh("straight_rail");
	meshRailRight = commonMeshes::getMesh("straight_rail");
	meshRailMM = commonMeshes::getMesh("straight_rail_mm");
	meshSleeper = commonMeshes::getMesh("sleeper");

	int nrSleepers = length / 0.8f;
	for (int i = 0; i <= nrSleepers; i++) {
		float t = (float)i / nrSleepers;
		glm::vec3 pos = GetPos(t);

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, pos);

		float angle = atan2(direction.x, direction.z);
		modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.05f, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(trackWidth + 0.4, 0.1f, 0.3f));

		sleeperTransforms.push_back(modelMatrix);
	}

	type = 0; // Straight rail
	bridgeMesh = nullptr;
	tunnelMesh = nullptr;

	bridgeMesh = commonMeshes::getMesh("bridge");
	tunnelMesh = commonMeshes::getMesh("tunnel");
}

glm::vec3 StraightRail::GetPos(float t) {
	return startPosition + t * (endPosition - startPosition);
}

glm::vec3 StraightRail::GetForwardVector(float t) {
	return glm::normalize(endPosition - startPosition);
}

float StraightRail::GetLength() {
	return glm::distance(startPosition, endPosition);
}

void StraightRail::RenderCosmetics(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glm::vec3 dir = GetForwardVector(0);
    float angle = atan2(dir.x, dir.z);
    for (auto groundUtil : groundUtils) {
        if (groundUtil.tileType == 1) { // bridge

			glm::mat4 modelBridge = glm::mat4(1.0f);
			modelBridge = glm::translate(modelBridge, groundUtil.translation);
			modelBridge = glm::translate(modelBridge, glm::vec3(0, -0.05f, 0));

			modelBridge = glm::rotate(modelBridge, angle, glm::vec3(0, 1, 0));
            modelBridge = glm::rotate(modelBridge, glm::radians(-90.0f), glm::vec3(0, 1, 0));

            modelBridge = glm::scale(modelBridge, glm::vec3(groundUtil.scale, 1.0f, 1.0f));
            modelBridge = glm::scale(modelBridge, glm::vec3(1.1f, 0.2f, trackWidth + 2.0f));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelBridge));
			bridgeMesh->Render();
		}
        else if (groundUtil.tileType == 2) { // tunnel
			glm::mat4 modelTunnel = glm::mat4(1.0f);
			modelTunnel = glm::translate(modelTunnel, groundUtil.translation);
			modelTunnel = glm::rotate(modelTunnel, angle, glm::vec3(0, 1, 0));
			modelTunnel = glm::rotate(modelTunnel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
			modelTunnel = glm::scale(modelTunnel, glm::vec3(1.0f, groundUtil.scale, 1.0f));
			modelTunnel = glm::scale(modelTunnel, glm::vec3(1.0f, 1.1f, 2.0f));
			glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelTunnel));
            tunnelMesh->Render();
        }
    }
}

void StraightRail::Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw Sleepers
    for (const auto& model : sleeperTransforms) {
        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
        meshSleeper->Render();
    }

    // Draw Left Rail
    glm::vec3 midPoint = (startPosition + endPosition) * 0.5f;
    glm::vec3 dir = GetForwardVector(0);
    glm::vec3 right = glm::cross(dir, glm::vec3(0, 1, 0));
    float angle = atan2(dir.x, dir.z);
    float length = GetLength();

    glm::mat4 modelRail = glm::mat4(1);
    modelRail = glm::translate(modelRail, midPoint - right * (trackWidth * 0.5f));
    modelRail = glm::translate(modelRail, glm::vec3(0, 0.15f, 0));
    modelRail = glm::rotate(modelRail, angle, glm::vec3(0, 1, 0));
    modelRail = glm::scale(modelRail, glm::vec3(0.1f, 0.1f, length));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelRail));
    meshRailLeft->Render();

    // Draw Right Rail
    modelRail = glm::mat4(1);
    modelRail = glm::translate(modelRail, midPoint + right * (trackWidth * 0.5f));
    modelRail = glm::translate(modelRail, glm::vec3(0, 0.15f, 0));
    modelRail = glm::rotate(modelRail, angle, glm::vec3(0, 1, 0));
    modelRail = glm::scale(modelRail, glm::vec3(0.1f, 0.1f, length));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelRail));
    meshRailRight->Render();

	RenderCosmetics(shader, viewMatrix, projectionMatrix);
}

void StraightRail::RenderMinimap(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glm::vec3 midPoint = (startPosition + endPosition) * 0.5f;
    glm::vec3 dir = GetForwardVector(0);
    glm::vec3 right = glm::cross(dir, glm::vec3(0, 1, 0));
    float angle = atan2(dir.x, dir.z);
    float length = GetLength();

    glm::mat4 modelRail = glm::mat4(1);
    modelRail = glm::translate(modelRail, midPoint - right * (trackWidth * 0.5f));
    modelRail = glm::rotate(modelRail, angle, glm::vec3(0, 1, 0));
    modelRail = glm::scale(modelRail, glm::vec3(1.0f, 10.0f, length));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelRail));
    meshRailMM->Render();
}

CurvedRail::CurvedRail(glm::vec3 center, float radius, float startAngleDeg, float sweepAngleDeg, float width, int constructionT)
    : centerPosition(center), radius(radius), trackWidth(width), constructionType(constructionT) {
	startAngleRad = glm::radians(startAngleDeg);
    sweepAngleRad = glm::radians(sweepAngleDeg);
	endAngleRad = startAngleRad + sweepAngleRad;

    meshRailInner = CreateBentRailMesh("inner_rail", radius - width / 2.0f, 0.1f, 0.1f, startAngleRad, endAngleRad, glm::vec3(0.7f, 0.7f, 0.7f));
    meshRailOuter = CreateBentRailMesh("outer_rail", radius + width / 2.0f, 0.1f, 0.1f, startAngleRad, endAngleRad, glm::vec3(0.7f, 0.7f, 0.7f));
	meshRailMM = CreateBentRailMesh("mm_rail", radius, 1.0f, 1.0f, startAngleRad, endAngleRad, glm::vec3(1.0f, 1.0f, 1.0f));
	meshSleeper = commonMeshes::getMesh("sleeper");

	float arcLength = radius * fabs(sweepAngleRad);
	int nrSleepers = arcLength / 0.8f;

    for (int i = 0; i <= nrSleepers; i++) {
		float t = (float)i / nrSleepers;
		float currAngle = startAngleRad + t * sweepAngleRad;

        glm::vec3 pos = glm::vec3(
            center.x + radius * cos(currAngle),
            center.y,
            center.z + radius * sin(currAngle)
        );

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, pos);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.05f, 0));
		modelMatrix = glm::rotate(modelMatrix, -currAngle, glm::vec3(0, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(trackWidth + 0.4f, 0.1f, 0.3f));
		sleeperTransforms.push_back(modelMatrix);
    }

	type = 1; // Curved rail
}

glm::vec3 CurvedRail::GetPos(float t) {
    float currentAngle = startAngleRad + sweepAngleRad * t;
    return glm::vec3(
        centerPosition.x + radius * cos(currentAngle),
        centerPosition.y,
        centerPosition.z + radius * sin(currentAngle)
    );
}

glm::vec3 CurvedRail::GetForwardVector(float t) {
    float currAngle = startAngleRad + sweepAngleRad * t;
	float dir = (sweepAngleRad >= 0) ? 1.0f : -1.0f;

    return glm::normalize(glm::vec3(
        -sin(currAngle) * dir,
        0,
        cos(currAngle) * dir
	));
}

float CurvedRail::GetLength() {
    return radius * std::abs(sweepAngleRad);
}

void CurvedRail::Render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    
    // Draw Sleepers
    for (const auto& model : sleeperTransforms) {
        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
        meshSleeper->Render();
    }

	// Draw Rails
	glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, centerPosition);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.15f, 0));

    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    meshRailInner->Render();
    meshRailOuter->Render();

	RenderCosmetics(shader, viewMatrix, projectionMatrix);
}

void CurvedRail::RenderMinimap(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, centerPosition);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 10.0f, 0));

    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    meshRailMM->Render();
}

void CurvedRail::RenderCosmetics(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    for (auto groundUtil : cosmeticMeshes) {
        if (groundUtil.tileType == 1) { // bridge
			glm::mat4 modelBridge = glm::mat4(1.0f);
			modelBridge = glm::translate(modelBridge, centerPosition);
			modelBridge = glm::translate(modelBridge, glm::vec3(0, -0.05f, 0)); 
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelBridge));
			groundUtil.mesh->Render();
		}
        else if (groundUtil.tileType == 2) { // tunnel
            glm::mat4 modelTunnel = glm::mat4(1.0f);
            modelTunnel = glm::translate(modelTunnel, centerPosition);
			modelTunnel = glm::scale(modelTunnel, glm::vec3(1.0f, 2.0f, 1.0f));
			glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelTunnel));
			groundUtil.mesh->Render();
        }
    }
}

Mesh* CurvedRail::CreateBentRailMesh(std::string name, float r, float w, float h, float startAng, float endAng, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    int slices = 32; // Smoothness
    float halfW = w / 2.0f;
    float halfH = h / 2.0f;

    for (int i = 0; i <= slices; i++) {
        float t = (float)i / slices;
        float theta = startAng + (endAng - startAng) * t;
        float cosT = cos(theta);
        float sinT = sin(theta);

        vertices.push_back(VertexFormat(glm::vec3((r - halfW) * cosT, -halfH, (r - halfW) * sinT), color));
        vertices.push_back(VertexFormat(glm::vec3((r + halfW) * cosT, -halfH, (r + halfW) * sinT), color));
        vertices.push_back(VertexFormat(glm::vec3((r + halfW) * cosT, halfH, (r + halfW) * sinT), color));
        vertices.push_back(VertexFormat(glm::vec3((r - halfW) * cosT, halfH, (r - halfW) * sinT), color));
    }

    for (int i = 0; i < slices; i++) {
        int start = i * 4;
        indices.push_back(start + 3); indices.push_back(start + 2); indices.push_back(start + 6);
        indices.push_back(start + 6); indices.push_back(start + 7); indices.push_back(start + 3);
        indices.push_back(start + 1); indices.push_back(start + 5); indices.push_back(start + 6);
        indices.push_back(start + 6); indices.push_back(start + 2); indices.push_back(start + 1);
        indices.push_back(start + 0); indices.push_back(start + 3); indices.push_back(start + 7);
        indices.push_back(start + 7); indices.push_back(start + 4); indices.push_back(start + 0);
    }

    Mesh* mesh = new Mesh(name);
    mesh->InitFromData(vertices, indices);
    return mesh;
}

CurvedRail* CreateCurvedRailFromPoints(glm::vec3 start, glm::vec3 end, float sweepAngleDeg, float width) {
    float dist = glm::distance(start, end);
    float angleRad = glm::radians(sweepAngleDeg);
    float halfAngle = angleRad / 2.0f;

    if (std::abs(sweepAngleDeg) < 1.0f || std::abs(sweepAngleDeg) >= 360.0f) {
        return nullptr;
    }

    float radius = dist / (2.0f * std::abs(sin(halfAngle)));
    float h = radius * cos(halfAngle);

    glm::vec3 midPoint = (start + end) * 0.5f;
    glm::vec3 dir = glm::normalize(end - start);
    glm::vec3 perpendicular = glm::normalize(glm::cross(dir, glm::vec3(0, 1, 0)));
    float sign = (sweepAngleDeg > 0) ? 1.0f : -1.0f;
    glm::vec3 center = midPoint + perpendicular * sign * h;

    float startAngle = glm::degrees(atan2(start.z - center.z, start.x - center.x));
    float normalizedStartAngle = fmod(startAngle + 360.0f, 360.0f);

    return new CurvedRail(center, radius, normalizedStartAngle, sweepAngleDeg, width);
}
