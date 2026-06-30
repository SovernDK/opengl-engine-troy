#pragma once
#include "rmui/ui_widget.h"

class SDL_Color;
class Texture2D;

class UIImage : public UIWidget
{
public:
	UIImage();
	UIImage(int id);
	~UIImage() = default;
};