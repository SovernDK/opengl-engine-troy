#pragma once
#include "rmui/ui.h"
#include "rmui/ui_layout.h"
#include "rmui/ui_widget.h"

#include "services/ui_service.h"

#include <resources.h>
#include <canvas_2d.h>

UIRect AbsoluteLayout::layout(UIWidget& self, const UIRect& parentRect, int index)
{
	UIRect rect{ 0 };
	rect.pos  = parentRect.pos + self.localRect.pos * parentRect.size;
	rect.size = self.localRect.size * parentRect.size;

	rect.pos -= self.pivot * rect.size;

	return rect;
}

UIRect HorizontalLayout::layout(UIWidget& self, const UIRect& parentRect, int index)
{
	UIRect rect;
	rect.size = self.localRect.size * parentRect.size;

	if (expand)
		rect.size.y = parentRect.size.y - margin.top - margin.bottom;

	if (fit)
	{
		int childrenSize = self.parent.lock()->children().size();
		int availableWidth = parentRect.size.x / childrenSize;
		int spacingTotal = spacing / std::max(1, childrenSize - 1);
		rect.size.x = availableWidth - spacingTotal - margin.right;
	}

	if (self.parent.expired() || index == 0)
	{
		rect.pos = glm::vec2(margin.left + parentRect.pos.x, margin.top + parentRect.pos.y);
		return rect;
	}

	UIRect prevChild{ 0 };
	int prevChildIndex = index - 1;
	prevChild = self.parent.lock()->children().at(prevChildIndex)->rect;

	float posX = prevChild.pos.x + prevChild.size.x + spacing;
	float posY = margin.top + parentRect.pos.y;
	rect.pos = glm::vec2(posX, posY);

	return rect;
}

UIRect VerticalLayout::layout(UIWidget& self, const UIRect& parentRect, int index)
{
	UIRect rect{ 0 };
	rect.size = self.localRect.size * parentRect.size;

	if (expand)
		rect.size.x = parentRect.size.x - margin.left - margin.right;
	
	if (fit)
	{
		int childrenSize = self.parent.lock()->children().size();
		int availableHeight = parentRect.size.y / childrenSize;
		int spacingTotal = spacing / std::max(1, childrenSize - 1);
		rect.size.y = availableHeight - spacingTotal - margin.bottom;
	}

	if (self.parent.expired() || index == 0)
	{
		rect.pos = glm::vec2(margin.left + parentRect.pos.x, margin.top + parentRect.pos.y);
		return rect;
	}

	UIRect prevChild{ 0 };
	int prevChildIndex = index - 1;
	prevChild = self.parent.lock()->children().at(prevChildIndex)->rect;

	float posX = margin.left + parentRect.pos.x;
	float posY = prevChild.pos.y + prevChild.size.y + spacing;
	rect.pos = glm::vec2(posX, posY);

	return rect;
}