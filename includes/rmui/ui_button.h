#pragma once
#include "rmui/ui_widget.h"
#include "ui_image.h"
#include "ui_label.h"

class SDL_Color;
class Texture2D;

class UIButton : public UIWidget
{
public:
	UIImage* image = nullptr;
	UILabel* label = nullptr;
public:
	UIButton();
	UIButton(int id);
	~UIButton() = default;

	void setKeepAspectRatio(bool keepAspect) { image->keepAspectRatio = keepAspect; }
	void setText(std::string& text) { label->text = text; }
};