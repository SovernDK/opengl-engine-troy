#pragma once
#include "rmui/ui_label.h"
#include "rmui/ui_interaction.h"

UILabel::UILabel() : UIWidget()
{
	interactive = false;
	blocking = false;
	interaction = std::make_unique<UIInteraction>();
}

UILabel::UILabel(int id) : UIWidget(id)
{
	interactive = false;
	blocking = false;
	interaction = std::make_unique<UIInteraction>();
}