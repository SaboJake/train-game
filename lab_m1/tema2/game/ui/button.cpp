#include "button.h"

#include <iostream>

void Button::Render(Shader* shader, const glm::mat4& modelMatrix, const glm::mat4& projectionMatrix) {
	shader->Use();
	glm::mat4 modelButton = modelMatrix;
	modelButton = glm::translate(modelButton, position);
	modelButton = glm::scale(modelButton, glm::vec3(width, height, 1));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelButton));
	glm::mat4 identityView = glm::mat4(1);
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(identityView));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	mesh->Render();
}

void Button::RenderText(gfxc::TextRenderer* textRenderer, const std::string& text, glm::vec3 textColor, float scale, float windowHeight) {
	float estimatedTextWidth = 60.0f;
	float textWidth = text.length() * estimatedTextWidth * scale;
	float textHalfWidth = textWidth / 2.0f;

	float textX = position.x - textHalfWidth;
	float textY = windowHeight - position.y - (60.0f * scale) / 2.0f;

	textRenderer->RenderText(text, textX, textY, scale, textColor);
}

bool Button::IsMouseOver(const glm::vec2& mousePos) const {
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	float adjustedX = position.x;
	float adjustedY = position.y;

	return (mousePos.x >= adjustedX - halfWidth && mousePos.x <= adjustedX + halfWidth &&
			mousePos.y >= adjustedY - halfHeight && mousePos.y <= adjustedY + halfHeight);
}
