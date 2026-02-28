#pragma once

#include "components/text_renderer.h"
#include "lab_m1/tema2/game/resource.h"

#include <glm/glm.hpp>
#include <vector>

class Stats {
public:
	Stats(gfxc::TextRenderer* textRenderer)
		: res(refRes), textRenderer(textRenderer), sore(0.0f),
		time(0.0f), timeSeconds(0.0f), timeMinutes(0), requestedResources({})
	{
	}

	void SetResolution(glm::vec2 resolution) {
		this->res = resolution;
	}
	glm::vec2 GetResolution() const {
		return res;
	}

	void SetTextRenderer(gfxc::TextRenderer* textRenderer) {
		this->textRenderer = textRenderer;
	}

	void SetScore(float score) {
		this->sore = score;
	}
	float GetScore() const {
		return sore;
	}

	void SetTime(float time) {
		this->time = time;
	}
	float GetTime() const {
		return time;
	}

	void SetRequestedResources(const std::vector<Resource*>& resources) {
		this->requestedResources = resources;
	}

	void Update(float deltaTime);

private:
	const glm::vec2 refRes = glm::vec2(1280, 720);
	glm::vec2 res;
	gfxc::TextRenderer* textRenderer;

	float sore;
	float time;

	float timeSeconds;
	int timeMinutes;

	std::vector<Resource*> requestedResources;
};
