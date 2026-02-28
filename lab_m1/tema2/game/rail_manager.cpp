#include "rail_manager.h"
#include "lab_m1/tema2/my_utils/ray_utils.h"

#include <iostream>
#include <glm/glm.hpp>

bool RailManager::CheckTrainsOnRail(Rail* rail) {
    for (Train* train : trains) {
        if (train->CheckRail(rail)) {
            return true;
		}
    }
    return false;
}

void RailManager::AddRail(Rail* rail) {
	ComputeCosmeticsForRail(rail);
	rails.push_back(rail);
}

int RailManager::GetGroundTileTypeAtPosition(glm::vec3 position) {
    for (auto ground : groundTiles) {
        if (ground->IsPositionInside(position)) {
            return ground->GetTileType();
        }
	}
    return 0;
}

void RailManager::ComputeCosmeticsForStraightRail(StraightRail* rail) {
	int nr = static_cast<int>(rail->GetLength() / 0.01f);
	glm::vec3 lastPos = rail->GetPos(0.0f);
	int lastTileType = GetGroundTileTypeAtPosition(lastPos);

	rail->ClearGroundUtils();

    for (int i = 0; i <= nr; i++) {
        float t = static_cast<float>(i) / nr;
        glm::vec3 pos = rail->GetPos(t);
		int tileType = GetGroundTileTypeAtPosition(pos);

        if (tileType != lastTileType) {
            GroundUtil gu;
			glm::vec3 centerPos = (lastPos + pos) * 0.5f;

			gu.translation = centerPos;
            gu.scale = glm::distance(pos, lastPos);
            gu.tileType = lastTileType;

			rail->AddGroundUtil(gu);

			lastPos = pos;
			lastTileType = tileType;
        }
    }

	GroundUtil gu;
	glm::vec3 centerPos = (lastPos + rail->GetPos(1.0f)) * 0.5f;

	gu.translation = centerPos;
    gu.scale = glm::distance(rail->GetPos(1.0f), lastPos);
	gu.tileType = lastTileType;

	rail->AddGroundUtil(gu);
}

void RailManager::ComputeCosmeticsForCurvedRail(CurvedRail* rail) {
    int nr = static_cast<int>(rail->GetLength() / 0.01f);
    glm::vec3 lastPos = rail->GetPos(0.0f);
    int lastTileType = GetGroundTileTypeAtPosition(lastPos);
    float lastT = 0.0f;
	rail->ClearCosmeticMeshes();

    float railStartAngle = rail->GetStartAngleDeg();
    float railSweep = rail->GetSweepAngleDeg();
    float radius = rail->GetRadius();
    float width = rail->GetTrackWidth() + 2.0f;

    for (int i = 0; i <= nr; i++) {
        float t = static_cast<float>(i) / nr;
        glm::vec3 pos = rail->GetPos(t);
        int tileType = GetGroundTileTypeAtPosition(pos);

        if (tileType != lastTileType) {
            Mesh* cosmeticMesh = nullptr;
            GroundUtilMesh gu;
            float segmentStartAngle = railStartAngle + railSweep * lastT;
            float segmentArc = railSweep * (t - lastT);
            if (lastTileType == 1) { // bridge

                // Calculate where this specific segment starts
                cosmeticMesh = object3D::CreateCurvedRail(
                    "curved_bridge_segment",
                    glm::vec3(0),
                    radius,
                    segmentStartAngle,
                    segmentArc,
                    width,
                    0.2f,
                    glm::vec3(0.55f, 0.27f, 0.07f)
                );
                gu.mesh = cosmeticMesh;
                gu.tileType = lastTileType;

                rail->AddCosmeticMesh(gu);

            }
            else if (lastTileType == 2) {
                cosmeticMesh = object3D::CreateCurvedTunnel(
                    "curved_tunnel_segment",
                    glm::vec3(0),
					radius,
					2.0f,
                    1.6f,
					segmentStartAngle,
                    segmentArc,
                    glm::vec3(0.3f, 0.1f, 0.1f)
				);
                    
				gu.mesh = cosmeticMesh;
                gu.tileType = lastTileType;
				rail->AddCosmeticMesh(gu);
            }
            lastPos = pos;
            lastTileType = tileType;
            lastT = t;
        }
    }

    // Handle the final segment
    if (lastTileType == 1) {
        float segmentStartAngle = railStartAngle + railSweep * lastT;
        float segmentArc = railSweep * (1.0f - lastT);

        Mesh* cosmeticMesh = object3D::CreateCurvedRail(
            "curved_bridge_segment_end",
            glm::vec3(0),
            radius,
            segmentStartAngle,
            segmentArc,
            width,
            0.2f,
            glm::vec3(0.55f, 0.27f, 0.07f)
        );

        GroundUtilMesh gu;
        gu.mesh = cosmeticMesh;
        gu.tileType = lastTileType;

        rail->AddCosmeticMesh(gu);
    }
    else if (lastTileType == 2) {
        float segmentStartAngle = railStartAngle + railSweep * lastT;
        float segmentArc = railSweep * (1.0f - lastT);

        Mesh* cosmeticMesh = object3D::CreateCurvedTunnel(
            "curved_tunnel_segment_end",
            glm::vec3(0),
            radius,
            2.0f,
            1.6f,
            segmentStartAngle,
            segmentArc,
            glm::vec3(0.3f, 0.1f, 0.1f)
        );

        GroundUtilMesh gu;
        gu.mesh = cosmeticMesh;
        gu.tileType = lastTileType;

        rail->AddCosmeticMesh(gu);
    }
}

void RailManager::ComputeCosmeticsForRail(Rail* rail) {
    if (rail->type == 0) { // Straight
        ComputeCosmeticsForStraightRail(dynamic_cast<StraightRail*>(rail));
    }
    else if (rail->type == 1) { // Curved
        ComputeCosmeticsForCurvedRail(dynamic_cast<CurvedRail*>(rail));
    }
}

void RailManager::RemoveRail(int index) {
	if (index < 0 || index >= rails.size()) return;

	Rail* railToRemove = rails[index];

    auto disconnect = [&](std::vector<Rail*>& list, int& switchIndex, Rail* rail) {
        for (int i = 0; i < list.size(); i++) {
            if (list[i] == rail) {
                list.erase(list.begin() + i);
                if (i < switchIndex) {
                    switchIndex--;
                }

                if (switchIndex >= list.size()) {
                    switchIndex = std::max(0, (int)list.size() - 1);
                }

                i--;
            }
        }
    };

    for (Rail* r : railToRemove->connectedStart) {
		disconnect(r->connectedStart, r->activeSwitchStart, railToRemove);
		disconnect(r->connectedEnd, r->activeSwitchEnd, railToRemove);
	}

    for (Rail* r : railToRemove->connectedEnd) {
        disconnect(r->connectedStart, r->activeSwitchStart, railToRemove);
        disconnect(r->connectedEnd, r->activeSwitchEnd, railToRemove);
	}

	delete railToRemove;
	rails.erase(rails.begin() + index);
}

void RailManager::RemoveTrain(int index) {
    if (index < 0 || index >= trains.size()) return;
    Train* trainToRemove = trains[index];
    delete trainToRemove;
    trains.erase(trains.begin() + index);
}

bool RailManager::CheckRailOccupied(Rail* rail) {
    for (Train* train : trains) {
        if (train->CheckRail(rail)) {
            return true;
		}
    }
	return false;
}

std::pair<Rail*, Rail*> RailManager::SplitRail(Rail* rail, float t) {
	Rail* part1 = nullptr;
	Rail* part2 = nullptr;

	glm::vec3 startPos = rail->GetPos(0.0f);
	glm::vec3 midPos = rail->GetPos(t);
	glm::vec3 endPos = rail->GetPos(1.0f);

	if (rail->type == 0) { // Straight
		part1 = new StraightRail(startPos, midPos);
		part2 = new StraightRail(midPos, endPos);
	}
    else if (rail->type == 1) { // Curved
		CurvedRail* curvedRail = dynamic_cast<CurvedRail*>(rail);
        float angle = curvedRail->GetSweepAngleDeg();
		part1 = CreateCurvedRailFromPoints(startPos, midPos, angle * t);
		part2 = CreateCurvedRailFromPoints(midPos, endPos, angle * (1.0f - t));
    }
    else {
        std::cerr << "Unknown rail type for splitting." << std::endl;
        return std::make_pair(nullptr, nullptr);
    }

    if (!part1 || !part2) {
        std::cerr << "Failed to create rail parts during split." << std::endl;
        return std::make_pair(nullptr, nullptr);
	}

	// Connect part1 and part2
	part1->ConnectTo(part2, true, false);
	
    auto ReconnectNeighbours = [&](std::vector<Rail*>& neighbours, Rail* oldRail, Rail* newRail, bool newRailEnd) {
        for (Rail* r : neighbours) {
            for (size_t i = 0; i < r->connectedStart.size(); i++) {
                if (r->connectedStart[i] == oldRail) {
                    bool wasActive = (r->activeSwitchStart == i);
                    Connect(r, false, newRail, newRailEnd);
                    if (wasActive) {
                        r->activeSwitchStart = r->connectedStart.size() - 1;
                    }
                    break;
                }
            }
            for (size_t i = 0; i < r->connectedEnd.size(); i++) {
                if (r->connectedEnd[i] == oldRail) {
                    bool wasActive = (r->activeSwitchEnd == i);
                    Connect(r, true, newRail, newRailEnd);
                    if (wasActive) {
                        r->activeSwitchEnd = r->connectedEnd.size() - 1;
                    }
                    break;
                }
            }
        }
    };

	ReconnectNeighbours(rail->connectedStart, rail, part1, false);
	ReconnectNeighbours(rail->connectedEnd, rail, part2, true);

	AddRail(part1);
	AddRail(part2);

    for (int i = 0; i < rails.size(); i++) {
        if (rails[i] == rail) {
            RemoveRail(i);
            break;
        }
	}
	return std::make_pair(part1, part2);
}

void RailManager::RenderAllRails(Shader* shader, Shader* highlightShader, int selectedIndex, int trackSwitching, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	Rail* neigh = nullptr;
    if (selectedIndex >= 0 && selectedIndex < rails.size()) {
        Rail* selectedRail = rails[selectedIndex];
        if (trackSwitching == 1) {
            if (!selectedRail->connectedEnd.empty()) {
                neigh = selectedRail->connectedEnd[selectedRail->activeSwitchEnd];
            }
        }
        else if (trackSwitching == -1) {
            if (!selectedRail->connectedStart.empty()) {
                neigh = selectedRail->connectedStart[selectedRail->activeSwitchStart];
            }
        }
    }
    for (int i = 0; i < rails.size(); i++) {
		Rail* r = rails[i];
		Shader* sh = (i == selectedIndex || r == neigh) ? highlightShader : shader;
		r->Render(sh, viewMatrix, projectionMatrix);
	}
    if (previewRail && buildMode) {
        previewRail->Render(shader, viewMatrix, projectionMatrix);
	}
    if (buildMode) {
        RenderDebug(shader, viewMatrix, projectionMatrix);
    }
}

int RailManager::GetRailIndex(glm::vec3 rayOrigin, glm::vec3 rayDirection) {
    float closest = std::numeric_limits<float>::max();
    int index = -1;

    glm::vec3 normalizedRayDir = glm::normalize(rayDirection);

    for (int i = 0; i < rails.size(); i++) {
        Rail* rail = rails[i];
        float railLength = rail->GetLength();
        int numSamples = static_cast<int>(railLength);

        for (int t = 0; t <= numSamples; t++) {
            float param = static_cast<float>(t) / numSamples;
            glm::vec3 point = rail->GetPos(param);

            float dist = RayUtils::IntersectSphere(rayOrigin, normalizedRayDir, point, 1.0f);
            if (dist >= 0.0f && dist < closest) {
				relativeT = param;
                closest = dist;
                index = i;
            }
        }
    }

	railIndex = index;

    if (index >= 0) {
		float step = unit / rails[index]->GetLength();
		relativeT = std::round(relativeT / step) * step;

    }

    return index;
}

int RailManager::GetTrainIndex(glm::vec3 rayOrigin, glm::vec3 rayDirection) {
	float closest = std::numeric_limits<float>::max();
    int index = -1;

	glm::vec3 normalizedRayDir = glm::normalize(rayDirection);

    for (int i = 0; i < trains.size(); i++) {
        Train* train = trains[i];
        float dist = train->GetCarDistace(rayOrigin, normalizedRayDir);
        if (dist >= 0.0f && dist < closest) {
            closest = dist;
            index = i;
        }
    }
    return index;
}

SnapPoint RailManager::GetClosestSnapPoint(glm::vec3 position, float threshold) {
    SnapPoint bestPoint;
    bestPoint.rail = nullptr;
    bestPoint.isValid = false;

    float minDistance = threshold;

    for (Rail* r : rails) {
        glm::vec3 startPos = r->GetPos(0.0f);
        float distStart = glm::distance(position, startPos);

        if (distStart < minDistance) {
            minDistance = distStart;
            bestPoint.rail = r;
            bestPoint.isEnd = false;
            bestPoint.position = startPos;
            bestPoint.isValid = true;
			bestPoint.t = 0.0f;
        }

        glm::vec3 endPos = r->GetPos(1.0f);
        float distEnd = glm::distance(position, endPos);

        if (distEnd < minDistance) {
            minDistance = distEnd;
            bestPoint.rail = r;
            bestPoint.isEnd = true;
            bestPoint.position = endPos;
            bestPoint.isValid = true;
			bestPoint.t = 1.0f;
        }
    }

    return bestPoint;
}

std::vector<SnapPoint> RailManager::GetSnapPoints(glm::vec3 position, float threshold) {
    std::vector<SnapPoint> points;

    for (Rail* r : rails) {
        glm::vec3 startPos = r->GetPos(0.0f);
        if (glm::distance(position, startPos) < threshold) {
            SnapPoint snapPoint;
            snapPoint.rail = r;
            snapPoint.isEnd = false;
            snapPoint.position = startPos;
            snapPoint.isValid = true;
            points.push_back(snapPoint);
        }

        glm::vec3 endPos = r->GetPos(1.0f);
        if (glm::distance(position, endPos) < threshold) {
            SnapPoint snapPoint;
            snapPoint.rail = r;
            snapPoint.isEnd = true;
            snapPoint.position = endPos;
            snapPoint.isValid = true;
            points.push_back(snapPoint);
        }
    }
    return points;
}

// Used for connecting to an existing rail (not ends)
SnapPoint RailManager::GetMiddleSnapPoint(glm::vec3 position, float threshold) {
	SnapPoint bestPoint;
	bestPoint.rail = nullptr;
	bestPoint.isValid = false;

	float minDistance = threshold;

    for (Rail* r : rails) {
        int nrSteps = static_cast<int>(r->GetLength() / unit);
        for (int i = 1; i < nrSteps; i++) {
            float t = static_cast<float>(i) / nrSteps;
            float dist = glm::distance(position, r->GetPos(t));

            if (dist < minDistance) {
                minDistance = dist;
                bestPoint.rail = r;
                bestPoint.isEnd = false; // Middle point
                bestPoint.position = r->GetPos(t);
                bestPoint.isValid = true;
				bestPoint.t = t;
			}
        }
    }

	return bestPoint;
}

glm::vec3 GetOutwardVector(Rail* r, bool isEnd) {
    glm::vec3 dir = r->GetForwardVector(isEnd ? 1.0f : 0.0f);
    return isEnd ? dir : -dir;
}

void RailManager::Connect(Rail* newRail, bool newRailEndIs1, Rail* targetRail, bool targetRailEndIs1) {
    if (!newRail || !targetRail) return;
    if (newRail == targetRail) return;

    glm::vec3 dir1 = glm::normalize(GetOutwardVector(newRail, newRailEndIs1));
    glm::vec3 dir2 = glm::normalize(GetOutwardVector(targetRail, targetRailEndIs1));

    float dotProd = glm::dot(dir1, dir2);

    if (dotProd > -0.5f) {
        std::cout << "Connection Rejected: Angle too sharp (" << dotProd << ")" << std::endl;
        return;
    }

    newRail->ConnectTo(targetRail, newRailEndIs1, targetRailEndIs1);
    std::cout << "Connected Rail ID " << newRail << " to " << targetRail << std::endl;
}

Rail* RailManager::CreateRail(glm::vec3 start, glm::vec3 end, float sweepAngleDeg, float width) {
    if (!buildMode) return nullptr;

    Rail* newRail = nullptr;
    if (!curvedMode || angle == 0.0f) {
        newRail = new StraightRail(start, end, 1.0f);
    }
    else {
        newRail = CreateCurvedRailFromPoints(start, end, sweepAngleDeg, width);
    }
    return newRail;
}

Rail* RailManager::CreateRail() {
	Rail* newRail = CreateRail(startPos, endPos, angle);
    if (newRail) {
		startSelected = false;
		endSelected = false;
	}
	return newRail;
}

void RailManager::BuildRailUtil(glm::vec3 &intersectionPoint) {
    if (!buildMode) return;

	SnapPoint middleSnap = GetMiddleSnapPoint(intersectionPoint, 2.0f);
	Rail* middleSnapRail = nullptr;

    SnapPoint snapPoint = GetClosestSnapPoint(intersectionPoint, 2.0f);

    Rail* currentSnapRail = nullptr;
    bool connectSnapIsEnd = false;

	bool connectedToMiddle = false;
    float t = -1.0;

    if (snapPoint.isValid) {
        intersectionPoint = snapPoint.position;
        currentSnapRail = snapPoint.rail;
        connectSnapIsEnd = snapPoint.isEnd;
        std::cout << "Snapping to rail at " << (connectSnapIsEnd ? "End" : "Start") << std::endl;
		t = snapPoint.t;
    }

    if (middleSnap.isValid) {
        intersectionPoint = middleSnap.position;
        currentSnapRail = middleSnap.rail;
        connectSnapIsEnd = false;
		std::cout << "Snapping to middle of rail" << std::endl;
		connectedToMiddle = true;
		t = middleSnap.t;
    }

    if (!startSelected) {
        startSelected = true;
        startPos = intersectionPoint;

        snapRailStart = currentSnapRail;
        snapRailIsEnd = connectSnapIsEnd;
        snapRailT = t;
    }
    else if (!endSelected) {
        if (currentSnapRail == snapRailStart && currentSnapRail) {
			std::cout << "Cannot connect rail to itself. Cancelling rail creation." << std::endl;
			delete previewRail;
            previewRail = nullptr;
            startSelected = false;
            endSelected = false;
            snapRailStart = nullptr;
			return;
        }
        endSelected = true;
        endPos = intersectionPoint;
        Rail* newRail = CreateRail();
		if (false) { // Unused intersection check
            std::cout << "Rail intersection detected. Cancelling rail creation." << std::endl;
            endSelected = false;
			delete previewRail;
			previewRail = nullptr;
            return;
		}

        if (snapRailStart) {
            if (CheckTrainsOnRail(snapRailStart)) {
				std::cout << "Cannot connect to rail with active trains. Skipping connection." << std::endl;
                delete previewRail;
                previewRail = nullptr;
                startSelected = false;
                endSelected = false;
                snapRailStart = nullptr;
                return;
            }
            else {
                // Connection is at the end of a rail
                if (snapRailT == 0.0f || snapRailT == 1.0f) {
                    std::vector<SnapPoint> startSnaps = GetSnapPoints(startPos, 0.1f);
                    for (const auto& snap : startSnaps) {
                        // Don't connect to itself
                        if (snap.rail != newRail) {
                            Connect(newRail, false, snap.rail, snap.isEnd);
                        }
                    }
                }
                // Connection is in the middle of a rail
                else {
                    // Split the rail at snapRailT
                    std::pair<Rail*, Rail*> splitRails = SplitRail(snapRailStart, snapRailT);
                    Connect(newRail, false, splitRails.first, true);
                    Connect(newRail, false, splitRails.second, false);
                }
            }
        }

        if (currentSnapRail) {
            if (CheckTrainsOnRail(currentSnapRail)) {
                std::cout << "Cannot connect to rail with active trains. Skipping connection." << std::endl;
                delete previewRail;
                previewRail = nullptr;
                return;
			}
            else {
                if (t == 0.0f || t == 1.0f) {
                    std::vector<SnapPoint> endSnaps = GetSnapPoints(intersectionPoint, 0.1f);
                    for (const auto& snap : endSnaps) {
                        if (snap.rail != newRail) {
                            Connect(newRail, true, snap.rail, snap.isEnd);
                        }
                    }
                }
                else {
                    // Split the rail at t
                    std::pair<Rail*, Rail*> splitRails = SplitRail(currentSnapRail, t);
                    Connect(newRail, true, splitRails.first, true);
                    Connect(newRail, true, splitRails.second, false);
                }
            }
        }

        AddRail(newRail);
        startSelected = false;
        endSelected = false;
        snapRailStart = nullptr;
    }
}

void RailManager::UpdatePreviewRail(glm::vec3 intersectionPoint) {
    if (previewRail) {
        delete previewRail;
        previewRail = nullptr;
    }
    if (buildMode && startSelected) {
		previewRail = CreateRail(startPos, intersectionPoint, angle);
		ComputeCosmeticsForRail(previewRail);
    }
}

void RailManager::ToggleBuildMode() {
    buildMode = !buildMode;
    if (!buildMode) {
        startSelected = false;
        endSelected = false;
        if (previewRail) {
            delete previewRail;
            previewRail = nullptr;
        }
	}
};

void RailManager::ToggleCurvedMode() {
    curvedMode = !curvedMode;
};

void RailManager::IncrementAngle(float delta) {
    angle += delta;
	if (angle >= 180.0f) angle = 180.0f;
}

void RailManager::DecrementAngle(float delta) {
    angle -= delta;
	if (angle <= -180.0f) angle = -180.0f;
}

void RailManager::RenderDebug(Shader* shader, const glm::mat4& view, const glm::mat4& proj) {
    for (Rail* r : rails) {
        glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, r->GetPos(0.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        shader->Use();
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
        debugSphere->Render();

        model = glm::mat4(1);
        model = glm::translate(model, r->GetPos(1.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
        debugSphere->Render();

		int nrSteps = static_cast<int>(r->GetLength() / unit);
        for (int i = 1; i < nrSteps; i++) {
            float t = static_cast<float>(i) / nrSteps;
            model = glm::mat4(1);
            model = glm::translate(model, r->GetPos(t));
            model = glm::scale(model, glm::vec3(0.5f));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
            middleDebugSphere->Render();
        }
    }
}
