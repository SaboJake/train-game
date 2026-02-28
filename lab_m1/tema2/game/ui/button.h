#pragma once

#include "components/text_renderer.h"
#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"

#include <glm/glm.hpp>

class Button {
public:
	Button(Mesh* mesh, glm::vec3 position, int width, int height)
		: mesh(mesh), position(position), width(width), height(height) {
	}

	bool IsMouseOver(const glm::vec2& mousePos) const;

	bool IsClicked() const { return clicked; }
	bool SetClicked(bool value) { return clicked = value; }
	void SetMesh(Mesh* newMesh) { mesh = newMesh; }

	void Render(Shader* shader, const glm::mat4& modelMatrix, const glm::mat4& projectionMatrix);
	void RenderText(gfxc::TextRenderer* textRenderer, const std::string& text, glm::vec3 textColor, float scale, float windowHeight);

	void SetPosition(const glm::vec3& newPosition) { position = newPosition; }
	void SetSize(int newWidth, int newHeight) { width = newWidth; height = newHeight; }

	glm::vec3 GetPosition() const { return position; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
private:
	Mesh* mesh;

	glm::vec3 position;
	int width;
	int height;

	bool clicked = false;
};
