#pragma once
#include "services/service.h"
#include "SDL3/SDL.h"

#include <unordered_map>
#include <string>

#include <nlohmann/json.hpp>

//struct StyleComponent
//{
//	std::string name;
//	std::string value;
//
//	SDL_Color backgroundColor;
//};
//
//class IUIStyleService : IService
//{
//public:
//	virtual ~IUIStyleService() = default;
//
//	virtual void load(nlohmann::json data) = 0;
//};
//
//class UIStyleService : public IUIStyleService
//{
//private:
//	std::unordered_map<std::string, StyleComponent> styles{};
//public:
//	UIStyleService() = default;
//	~UIStyleService();
//
//	void load(nlohmann::json data) override;
//};