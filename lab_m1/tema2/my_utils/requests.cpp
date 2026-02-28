#include "requests.h"

#include <iostream>

RequestManager::RequestManager() {
	// Initialize with some predefined requests
	requestList = {
		{{ ResourceFactory::CreateWoodResource(), ResourceFactory::CreateIronResource() },
			{ 5, 3 }
		},
		{
			{ ResourceFactory::CreatePlanksResource(), ResourceFactory::CreateNailsResource() },
			{ 4, 5 }
		},
		{
			{ ResourceFactory::CreateSteelResource() },
			{ 6 }
		},
		{
			{ ResourceFactory::CreateDoorsResource() },
			{ 3 }
		},
		{
			{ ResourceFactory::CreateWoodResource(), ResourceFactory::CreateIronResource(),
			  ResourceFactory::CreatePlanksResource(), ResourceFactory::CreateSteelResource()},
			{ 3, 2, 5, 4 }
		},
		{
			{ ResourceFactory::CreateDoorsResource(), ResourceFactory::CreateSteelResource() },
			{ 4, 6 }
		}
	};
	currentRequestIndex = 0;
}

void RequestManager::NextLoop() {
	int maxResources = 10;
	for (int i = 0; i < requestList.size(); i++) {
		for (int j = 0; j < requestList[i].requestedAmounts.size(); j++) {
			requestList[i].requestedAmounts[j] += 1;
			if (requestList[i].requestedAmounts[j] > maxResources) {
				requestList[i].requestedAmounts[j] = maxResources;
			}
		}
	}
}

void RequestManager::PrintRequests() {
	for (size_t i = 0; i < requestList.size(); i++) {
		std::cout << "Request " << i << ": ";
		for (size_t j = 0; j < requestList[i].acceptedResources.size(); j++) {
			std::cout << ResourceTypeToString(requestList[i].acceptedResources[j]->GetType()) << " x" << requestList[i].requestedAmounts[j] << " ";
		}
		std::cout << std::endl;
	}
}

RequestUtil RequestManager::GetNextRequestUtil() {
	RequestUtil request;
	for (auto res : requestList[currentRequestIndex].acceptedResources) {
		Resource* newRes = ResourceFactory::CreateResourceByType(res->GetType());
		request.acceptedResources.push_back(newRes);
	}
	for (auto amt : requestList[currentRequestIndex].requestedAmounts) {
		request.requestedAmounts.push_back(amt);
	}
	currentRequestIndex = (currentRequestIndex + 1) % requestList.size();
	if (currentRequestIndex == 0) {
		NextLoop();
	}
	std::cout << "RequestManager: Providing next request index " << currentRequestIndex << std::endl;
	PrintRequests();
	return request;
}

RequestUtil RequestManager::GetRandomRequestUtil() {
	int randomIndex = rand() % requestList.size();
	return requestList[randomIndex];
}

void RequestManager::GetNewRequest(std::vector<Resource*>& outResources, std::vector<int>& outAmounts, bool random) {
	RequestUtil request;
	if (random) {
		request = GetRandomRequestUtil();
	} else {
		request = GetNextRequestUtil();
	}
	for (auto res : request.acceptedResources) {
		outResources.push_back(res);
	}
	for (auto amt : request.requestedAmounts) {
		outAmounts.push_back(amt);
	}
}
