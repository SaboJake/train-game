#include "stats.h"
#include "lab_m1/tema2/my_utils/common_meshes.h"

#include <string>
#include <sstream>
#include <iomanip>

void Stats::Update(float deltaTime) {
    if (!textRenderer) return;
    time += deltaTime;
    timeSeconds += deltaTime;
    if (timeSeconds >= 59.99f) {
        timeMinutes++;
        timeSeconds = 0.0f;
    }
    float scaleFactor = res.y / refRes.y;

    float textScale = 0.6f * scaleFactor;

    float leftMargin = 20.0f * scaleFactor;
    float topMargin = 50.0f * scaleFactor;
    float lineSpacing = 30.0f * scaleFactor;

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    std::string scoreStr = "Score: " + std::to_string((int)sore);
    textRenderer->RenderText(scoreStr, leftMargin, topMargin, textScale, color);

    std::stringstream timeSS;
    if (timeMinutes > 0) {
        timeSS << "Time: " << timeMinutes << "m "
            << std::fixed << std::setprecision(1) << timeSeconds << "s";
    }
    else {
        timeSS << "Time: "
            << std::fixed << std::setprecision(1) << timeSeconds << "s";
    }

    textRenderer->RenderText(timeSS.str(), leftMargin, topMargin + lineSpacing, textScale, color);
}
