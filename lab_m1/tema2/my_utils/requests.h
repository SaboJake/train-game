#pragma once

#include "lab_m1/tema2/game/resource.h"

#include <vector>

struct RequestUtil {
	std::vector<Resource*> acceptedResources;
	std::vector<int> requestedAmounts;
};

class RequestManager {
public:
	RequestManager();
	void GetNewRequest(std::vector<Resource*>& outResources, std::vector<int>& outAmounts, bool random = false);
	void IncrementRequestIndex() { currentRequestIndex = (currentRequestIndex + 1) % requestList.size(); }
private:
	std::vector<RequestUtil> requestList;
	int currentRequestIndex;

	RequestUtil GetNextRequestUtil();
	RequestUtil GetRandomRequestUtil();

	void NextLoop();

	void PrintRequests();
};
