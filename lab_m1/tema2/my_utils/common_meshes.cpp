#include "lab_m1/tema2/my_utils/common_meshes.h"

namespace commonMeshes {
    std::unordered_map<std::string, Mesh*> meshes;

    void InitMeshes() {
        meshes["straight_rail"] = object3D::CreateCube("straight_rail_l", glm::vec3(0), 1.0f, glm::vec3(0.7f, 0.7f, 0.7f), true);
		meshes["straight_rail_mm"] = object3D::CreateQuad("straight_rail_mm", glm::vec3(0), 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), true);

        meshes["sleeper"] = object3D::CreateCube("sleeper", glm::vec3(0), 1.0f, glm::vec3(0.4f, 0.2f, 0.1f), true);
		meshes["iron"] = object3D::CreateCube("iron", glm::vec3(0), 1.0f, glm::vec3(0.5f, 0.5f, 0.5f), true);
		meshes["wood"] = object3D::CreateHalfCylinder("wood", glm::vec3(0), 0.5f, 1.0f, glm::vec3(0.6f, 0.4f, 0.2f), true, 32);
		meshes["planks"] = object3D::CreateCube("planks", glm::vec3(0), 1.0f, glm::vec3(0.7, 0.5f, 0.3f), true);
		meshes["nails"] = object3D::CreateCone("nails", glm::vec3(0), 0.2f, 1.0f, glm::vec3(0.7f, 0.7f, 0.7f), true, 16);
		meshes["steel"] = object3D::CreateCube("steel", glm::vec3(0), 1.0f, glm::vec3(0.6f, 0.6f, 0.6f), true);
		meshes["doors"] = object3D::CreateCube("doors", glm::vec3(0), 1.0f, glm::vec3(0.5f, 0.3f, 0.1f), true);

        meshes["train_body"] = object3D::CreateCube("train_body", glm::vec3(0, 0, 0), 1.0f, glm::vec3(0.8f, 0.1f, 0.1f), true);
        meshes["train_boiler"] = object3D::CreateCylinder("train_boiler", glm::vec3(0, 0, 0), 0.5f, 1.0f, glm::vec3(0.8f, 0.1f, 0.1f), true, 32);
        meshes["train_cabin"] = object3D::CreateCube("train_cabin", glm::vec3(0, 0, 0), 1.0f, glm::vec3(0.1f, 0.1f, 0.5f), true);
        meshes["train_wheel"] = object3D::CreateCylinder("train_wheel", glm::vec3(0, 0, 0), 0.5f, 1.0f, glm::vec3(0.1f, 0.1f, 0.1f), true, 16);

		meshes["button_green"] = object3D::CreateRectangle("button_green", glm::vec3(0), 1.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f), true);
		meshes["button_red"] = object3D::CreateRectangle("button_red", glm::vec3(0), 1.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f), true);
		meshes["button_yellow"] = object3D::CreateRectangle("button_yellow", glm::vec3(0), 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0), true);

        meshes["bridge"] = object3D::CreateCube("bridge", glm::vec3(0), 1.0, glm::vec3(0.55f, 0.27f, 0.07f), true);
        meshes["tunnel"] = object3D::CreateHollowHalfCylinder("tunnel", glm::vec3(0), 2.0f, 1.6f, 1.0f, glm::vec3(0.3f, 0.1f, 0.1f), 36);

		meshes["station1_base"] = object3D::CreateCube("station1_base", glm::vec3(0), 1.0f, glm::vec3(1.0f, 0.7f, 0.5f), true);
        meshes["station1_roof"] = object3D::CreatePyramid("station1_roof", glm::vec3(0), 1.0f, 1.0f, glm::vec3(0.3f, 0.1f, 0.05f), true);

		meshes["station2_base"] = object3D::CreateCube("station2_base", glm::vec3(0), 1.0f, glm::vec3(1.0f, 0.7f, 0.5f), true);
		meshes["station2_roof"] = object3D::CreateCube("station2_roof", glm::vec3(0), 1.0f, glm::vec3(0.3f, 0.1f, 0.05f), true);
		meshes["station2_pillar"] = object3D::CreateCylinder("station2_pillar", glm::vec3(0), 0.1f, 1.0f, glm::vec3(0.8f, 0.8f, 0.8f), true, 16);
    }

    Mesh* getMesh(const std::string& meshID) {
        auto it = meshes.find(meshID);
        if (it != meshes.end()) {
            return it->second;
        }
        return nullptr;
    }
}
