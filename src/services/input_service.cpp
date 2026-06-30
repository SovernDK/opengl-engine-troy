#pragma once
#include "services/input_service.h"
#include "SDL3/SDL_Log.h"
#include "utility/utils.h"

void InputService::load(nlohmann::json data)
{
	for (auto& [action, val] : data["inputs"].items())
	{
		for (auto& binding : val)
		{
			std::string device	= binding.value("device", "");
			std::string key		= binding.value("key", "");
			std::string event	= binding.value("event", "");

			if (device == "" || key == "" || event == "")
			{
				SDL_Log("Error while loading input: device=%s, key=%s, event=%s", device.c_str(), key.c_str(), event.c_str());
				continue;
			};

			uint8_t deviceId = 0;
			uint8_t button = 0;
			SDL_Keycode keycode = SDLK_UNKNOWN;
			SDL_EventType keyEvent	= SDL_EVENT_KEY_UP;

			if (device == "mouse")
			{
				deviceId = 1;
				if (text::toLower(key) == "left")		 button = SDL_BUTTON_LEFT;
				else if (text::toLower(key) == "middle") button = SDL_BUTTON_MIDDLE;
				else if (text::toLower(key) == "right")	 button = SDL_BUTTON_RIGHT;
			}
			else if(device == "keyboard")
			{
				deviceId = 0;
				keycode = SDL_GetKeyFromName(text::toLower(key).c_str());
				if (keycode == SDLK_UNKNOWN)
					SDL_Log("Cannot find key by code: %s", key.c_str());
			}

			if (device == "keyboard" && event == "up")		  keyEvent = SDL_EVENT_KEY_UP;
			else if (device == "keyboard" && event == "down") keyEvent = SDL_EVENT_KEY_DOWN;
			else if (device == "mouse" && event == "up")	  keyEvent = SDL_EVENT_MOUSE_BUTTON_UP;
			else if (device == "mouse" && event == "down")	  keyEvent = SDL_EVENT_MOUSE_BUTTON_DOWN;

			bindings[action].push_back({ keycode, keyEvent, button, deviceId });
		}
	}
}

glm::vec2 InputService::getMousePos()
{
	glm::vec2 out{ 0 };
	SDL_GetMouseState(&out.x, &out.y);
	return out;
}

bool InputService::getAction(std::string action)
{
	return actions[action];
}

void InputService::processEvents(SDL_Event& event)
{
	for (const auto& [action, bindings] : bindings)
	{
		for (const auto& bind : bindings)
		{
			if (bind.device == 0)
			{
				if (event.type == bind.eventType && event.key.key == bind.key)
					actions[action] = true;
			}
			else if (bind.device == 1)
			{
				if (event.type == bind.eventType && event.button.button == bind.button)
					actions[action] = true;
			}
		}
	}
}

void InputService::reset()
{
	for (const auto& [action, bindings] : bindings)
	{
		actions[action] = false;
	}
}
