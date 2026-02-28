#pragma once

#include "lab_m1/tema2/my_utils/object3D.h"
#include <unordered_map>
#include <string>

namespace commonMeshes {
    extern std::unordered_map<std::string, Mesh*> meshes;

    void InitMeshes();

    Mesh* getMesh(const std::string& meshID);
}
