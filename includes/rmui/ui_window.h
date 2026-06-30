#pragma once
#include "rmui/ui_widget.h"

class SDL_Color;

class UIWindow : public UIWidget
{
public:
	UIWindow() : UIWidget() {};
	UIWindow(int id) : UIWidget(id) {};
	~UIWindow();
};