#pragma once
#include "rmui/factories/ui_factory.h"

#include "services/ui_service.h"
#include <rmui/ui_image.h>

std::shared_ptr<UIWindow> UIWindowFactory::build(const std::string& handle)
{
	auto window = service.create<UIWindow>(handle, m_style, m_parent);
	window->setLocalRect(localRect);
	window->setPivot(pivot);

	return window;
}

std::shared_ptr<UIButton> UIButtonFactory::build(const std::string& handle)
{
	auto button = service.create<UIButton>(handle, m_style, m_parent);

	button->setLocalRect(localRect);
	button->setPivot(pivot);

	auto image = service.create<UIImage>(handle + "_img", "defImage", button);
	image->textureId = m_image;
	image->setLocalPosition(UIAnchor::Center);
	image->setLocalSize(1.0f);
	image->setPivot(UIAnchor::Center);
	image->matInst->blendMode = BlendMode::Alpha;
	image->keepAspectRatio = m_keepAspectRatio;
	button->image = image.get();

	if (!m_text.empty())
	{
		auto label = service.create<UILabel>(handle + "_label", "defText", button);
		label->setLocalPosition(UIAnchor::Center);
		label->setLocalSize(1.0f);
		label->setPivot(UIAnchor::Center);
		label->matInst->blendMode = BlendMode::Alpha;
		label->text = m_text;
		label->hasText = true;
		button->label = label.get();
	}
	
	if(onClick)		 button->interaction->setOnClick(onClick);
	if(onPressed)	 button->interaction->setOnPressed(onPressed);
	if(onEnterHover) button->interaction->setOnEnterHover(onEnterHover);
	if(onExitHover)	 button->interaction->setOnExitHover(onExitHover);

	return button;
}