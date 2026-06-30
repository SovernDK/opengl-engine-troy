#pragma once
#include "rmui/ui_button.h"
#include "rmui/ui_interaction.h"

UIButton::UIButton() : UIWidget()
{
	interactive = true;
	interaction = std::make_unique<UIInteraction>();
}

UIButton::UIButton(int id) : UIWidget(id)
{
	interactive = true;
	interaction = std::make_unique<UIInteraction>();
}