#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace RayUtils {
	inline glm::vec3 GetMouseRay(int mouseX, int mouseY, int screenWidth, int screenHeight,
		glm::mat4& viewMatrix, glm::mat4& projectionMatrix) {
		float x = (2.0f * mouseX) / screenWidth - 1.0f;
		float y = 1.0f - (2.0f * mouseY) / screenHeight;
		float z = 1.0f;

		glm::vec3 rayNDS = glm::vec3(x, y, z);
		glm::vec4 rayClip = glm::vec4(rayNDS.x, rayNDS.y, -1.0f, 1.0f);

		glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
		rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

		glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
		return glm::normalize(rayWorld);
	}

	inline float IntersectSphere(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 sphereCenter, float sphereRadius) {
		glm::vec3 dif = rayOrigin - sphereCenter;
		float b = glm::dot(dif, rayDirection);
		float c = glm::dot(dif, dif) - sphereRadius * sphereRadius;
		float discriminant = b * b - c;

		if (discriminant < 0) {
			return -1.0f; 
		}
		else {
			return -b - sqrt(discriminant);
		}
	}

	inline float IntersectPlane(glm::vec3 rayOrigin, glm::vec3 rayDirection, float planeHeight = 0.0f) {
		glm::vec3 planeNormal = glm::vec3(0.0f, 1.0f, 0.0f);
		float denom = glm::dot(planeNormal, rayDirection);

		if (abs(denom) > 1e-6) {
			glm::vec3 planePoint = glm::vec3(0.0f, planeHeight, 0.0f);
			float t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;
			if (t >= 0) {
				return t;
			}
		}
		return -1.0f;
	}
}
