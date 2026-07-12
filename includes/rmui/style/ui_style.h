#pragma once
#include "rmui/ui_layout.h"

struct StyleComponent
{
	std::string name = "def";

	SDL_Color bgColor = WHITE;
	SDL_Color hoverColor = WHITE;

	int fontSize = 24;
	std::string fontName = "default";
	SDL_Color fontColor = WHITE;
	SDL_Color fontHoverColor = RED;
	TextAlign align = TextAlign::Left;
	TextVertAlign valign = TextVertAlign::Top;

	std::unique_ptr<ILayoutStrategy>
		layoutStrategy = nullptr;

	Texture2D* texture = nullptr;

	bool dropShadow = false;
	glm::vec2 shadowOffset = glm::vec2(0);
};
