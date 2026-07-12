#pragma once
#include "rmui/factories/ui_factory.h"

#include "services/ui_service.h"
#include <services/audio_service.h>

std::shared_ptr<UIWidget> UILabelFactory::build(const std::string& handle)
{
	auto label = service.create<UIWidget>(handle, m_style, m_parent);
	label->setLocalRect(localRect);
	label->setPivot(pivot);
	
	label->addComponent<UIText>(UIText{ m_text });

	return label;
}

std::shared_ptr<UIWidget> UIWindowFactory::build(const std::string& handle)
{
	auto window = service.create<UIWidget>(handle, m_style, m_parent);
	window->setLocalRect(localRect);
	window->setPivot(pivot);

	window->addComponent<UIBackground>(UIBackground{ bgTexture, false });
	window->addComponent<UIDropShadow>();

	return window;
}

std::shared_ptr<UIWidget> UIButtonFactory::build(const std::string& handle)
{
	auto button = service.create<UIButton>(handle, m_style, m_parent);

	button->setLocalRect(localRect);
	button->setPivot(pivot);

	button->interactive = true;

	button->addComponent<UIBackground>(UIBackground{ bgTexture, false });
	button->addComponent<UIDropShadow>();

	button->interaction->addOnEnterHover([&](UIWidget* widget)
	{
		ServiceLocator::get<IAudioService>()->playOnce("clip");
	});

	if (!m_text.empty())
	{
		auto label = service.create<UIWidget>(handle + "_lbl", m_style, button);
		label->setLocalRect(UIRect());
		label->setPivot(UIAnchor::Top_Left);

		label->interactive = true;
		label->blocking = false;

		label->addComponent<UIText>(UIText{ m_text });
		button->label = label.get();

		button->interaction->addOnEnterHover([&](UIWidget* widget) { static_cast<UIButton*>(widget)->label->interaction->hovered = true; });
		button->interaction->addOnExitHover([&](UIWidget* widget) { static_cast<UIButton*>(widget)->label->interaction->hovered = false; });
	}

	if(onClick)		 button->interaction->addOnClick(onClick);
	if(onPressed)	 button->interaction->addOnPressed(onPressed);
	if(onEnterHover) button->interaction->addOnEnterHover(onEnterHover);
	if(onExitHover)	 button->interaction->addOnExitHover(onExitHover);

	return button;
}