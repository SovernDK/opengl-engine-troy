#pragma once
#include "rmui/ui_widget.h"
#include "glm/glm.hpp"
#include <functional>

class UIWidget;

class UIInteraction
{
private:
	std::function<void(UIWidget*)> onClick;
	std::function<void(UIWidget*, glm::vec2)> onPressed;
	std::function<void(UIWidget*)> onEnterHover;
	std::function<void(UIWidget*)> onExitHover;
public:
	bool hovered = false;
public:
	UIInteraction() = default;
	UIInteraction(const UIInteraction& other) = delete;
	UIInteraction& operator=(const UIInteraction& other) = delete;

	~UIInteraction() = default;

	void setOnClick(const std::function<void(UIWidget*)>& callback) { onClick = callback; }
	void setOnPressed(const std::function<void(UIWidget*, glm::vec2)>& callback) { onPressed = callback; }
	void setOnEnterHover(const std::function<void(UIWidget*)>& callback) { onEnterHover = callback;  }
	void setOnExitHover(const std::function<void(UIWidget*)>& callback) { onExitHover = callback;  }

	void triggerOnClick(UIWidget* widget) { if (onClick) onClick(widget); }
	void triggerOnPressed(UIWidget* widget, glm::vec2 mousePos) { if (onPressed) onPressed(widget, mousePos); }
	void triggerOnEnterHover(UIWidget* widget) { if (onEnterHover) onEnterHover(widget); hovered = true; }
	void triggerOnExitHover(UIWidget* widget) { if (onExitHover) onExitHover(widget); hovered = false; }
};