#pragma once
#include "services/ui_service.h"
#include "rmui/ui_window.h"
#include "graphics/material.h"
#include "utility/utils.h"

#include "services/service_locator.h"
#include "services/input_service.h"
#include "rmui/ui_interaction.h"
#include "rmui/ui_button.h"
#include "services/log_service.h"
#include "graphics/graphics.h"

#include "graphics/rendering/canvas_2d.h"

using json = nlohmann::json;

void UIService::loadStyles(json data)
{
	for (auto& [key, val] : data.items())
	{
		std::unique_ptr<StyleComponent> style = std::make_unique<StyleComponent>();
		style->name = key;

		std::string hexBGColor = val.value("backgroundColor", "#000000");
		std::string hexHoverColor = val.value("hoverColor", "#000000");
		uint8_t alpha = val.value("alpha", 255);

		SDL_Color bgColor = HexToRGBA(hexBGColor, alpha);
		SDL_Color hoverColor = HexToRGBA(hexHoverColor, alpha);

		style->bgColor = bgColor;
		style->hoverColor = hoverColor;

		if (val.contains("horizontalLayout"))
		{
			Margin margin{ 0 };
			margin.left		= val["horizontalLayout"]["margin"].value("left", 0);
			margin.right	= val["horizontalLayout"]["margin"].value("right", 0);
			margin.top		= val["horizontalLayout"]["margin"].value("top", 0);
			margin.bottom	= val["horizontalLayout"]["margin"].value("bottom", 0);

			std::unique_ptr<HorizontalLayout> layout = std::make_unique<HorizontalLayout>();

			layout->margin	= margin;
			layout->spacing = val["horizontalLayout"].value("spacing", 0);
			layout->expand	= val["horizontalLayout"].value("expand", false);
			layout->fit		= val["horizontalLayout"].value("fit", false);

			style->layoutStrategy = std::move(layout);
		}
		else if (val.contains("verticalLayout"))
		{
			Margin margin{ 0 };
			margin.left		= val["verticalLayout"]["margin"].value("left", 0);
			margin.right	= val["verticalLayout"]["margin"].value("right", 0);
			margin.top		= val["verticalLayout"]["margin"].value("top", 0);
			margin.bottom	= val["verticalLayout"]["margin"].value("bottom", 0);

			style->layoutStrategy = std::make_unique<VerticalLayout>();

			std::unique_ptr<VerticalLayout> layout = std::make_unique<VerticalLayout>();

			layout->margin	= margin;
			layout->spacing = val["verticalLayout"].value("spacing", 0);
			layout->expand	= val["verticalLayout"].value("expand", false);
			layout->fit		= val["verticalLayout"].value("fit", false);

			style->layoutStrategy = std::move(layout);
		}
		else
		{
			style->layoutStrategy = std::make_unique<AbsoluteLayout>();
		}

		if (val.contains("dropShadow"))
		{
			style->dropShadow		= val["dropShadow"].get<bool>();
			style->shadowOffset.x	= val["shadowOffset"].value("x", 1);
			style->shadowOffset.y	= val["shadowOffset"].value("y", 1);
		}

		styles[key] = std::move(style);
	}
}

void UIService::realizeStyle(UIWidget& widget)
{
	auto it = styles.find(widget.style);
	if (it == styles.end() || !it->second)
	{
		SDL_Log("Style '%s' not found for widget with id %d, using default style", widget.style.c_str(), widget.id);
		return;
	}
	auto& style = *it->second;

	SDL_Color color = style.bgColor;

	if(widget.interaction) 
	{
		color = widget.interaction->hovered ? style.hoverColor : style.bgColor;
	};

	if (style.dropShadow)
	{
		widget.shadowInst->setProperty("mainColor", glm::vec4(glm::vec3(0.0f), 1.0f));
		widget.shadowInst->setProperty("useTexture", false);

		glm::vec4 shadowRect = widget.rect.renderRect() + glm::vec4(style.shadowOffset, 0, 0);
		Canvas2D::drawQuad(shadowRect, widget.shadowInst.get());
	}

	widget.matInst->setProperty("mainColor", color::SDLColorToVec4(color));

	if (widget.useTexture)
	{
		widget.matInst->setTexture("image", widget.textureId.id);
		widget.matInst->setProperty("useTexture", true);
		Canvas2D::drawQuad(widget.rect.renderRect(), widget.matInst.get());
	}
	else
	{
		widget.matInst->setProperty("useTexture", false);
		Canvas2D::drawQuad(widget.rect.renderRect(), widget.matInst.get());
	}

	// Rework to check widget type and move widget specific data there
	if (widget.hasText)
	{
		ecs::Transform2D trans{ .position = glm::vec3(widget.rect.pos, 1.0f) };
		Canvas2D::drawText(widget.text, trans, 64);
	}

	Canvas2D::clear();
}

void UIService::resizeCanvas(int width, int height)
{
	canvasWidth = width;
	canvasHeight = height;

	m_root = std::make_shared<UIWidget>();
	m_root->id = idPool.getId();
	m_root->rect = UIRect(0, 0, canvasWidth, canvasHeight);
	m_root->setLocalPosition(0.0f, 0.0f);
	m_root->setLocalSize(1.0f, 1.0f);
	m_root->matInst = std::make_unique<MaterialInstance>(*materialInstance);
	m_root->shadowInst = std::make_unique<MaterialInstance>(*shadowInstance);
	m_root->style = "root";

	ids[m_root->id] = m_root;
	handles["root"] = m_root;

	m_root->dirtyUpdate = true;
}

void UIService::draw()
{
	if (!m_root) 
		return;

	//Add submission index for rendering in proper order
	submissionIndex = submissionStart;
	drawRecursive(m_root.get(), m_root->rect.renderRect());
	Canvas2D::clear();
}

void UIService::update()
{
	if (m_root->dirtyUpdate)
	{
		AbsoluteLayout rootStrategy{};
		updateRecursive(m_root.get(), UIRect(0, 0, canvasWidth, canvasHeight), rootStrategy, 0);
	}
}

void UIService::handleMouse(glm::vec2 mousePos)
{
	isBlocked	= false;
	focused		= nullptr;

	topWidgetAtPos(m_root.get(), mousePos);

	if (prevFocused && prevFocused != focused)
	{
		if (prevFocused->interactive)
			prevFocused->interaction->triggerOnExitHover(prevFocused);
		prevFocused = nullptr;
	}

	if (focused)
	{
		if(focused->interactive && !focused->interaction->hovered)
			focused->interaction->triggerOnEnterHover(focused);

		if (focused->interactive && ServiceLocator::get<IInputService>()->getAction("accept"))
		{
			focused->interaction->triggerOnClick(focused);
		}

		prevFocused = focused;
	}
}

void UIService::handleInput(SDL_Event& e)
{

}

void UIService::registerMats(const std::shared_ptr<Material>& mat, const std::shared_ptr<Material>& shadowMat)
{
	materialInstance = std::make_unique<MaterialInstance>(mat);

	materialInstance->blendMode = BlendMode::Alpha;
	materialInstance->setProperty("mainColor", glm::vec4(1.0f));

	shadowInstance = std::make_unique<MaterialInstance>(shadowMat);

	shadowInstance->blendMode = BlendMode::Alpha;
	shadowInstance->setProperty("mainColor", glm::vec4(glm::vec3(0.0f), 1.0f));
}

void UIService::registerMats(Material mat, Material shadowMat)
{
	auto material = std::make_shared<Material>(mat);
	materialInstance = std::make_unique<MaterialInstance>(material);

	materialInstance->blendMode = BlendMode::Alpha;
	materialInstance->setProperty("mainColor", glm::vec4(1.0f));

	auto dropShadow = std::make_shared<Material>(shadowMat);
	shadowInstance = std::make_unique<MaterialInstance>(dropShadow);

	shadowInstance->blendMode = BlendMode::Alpha;
	shadowInstance->setProperty("mainColor", glm::vec4(glm::vec3(0.0f), 1.0f));
}

void UIService::drawRecursive(UIWidget* widget, const glm::vec4 parentClip)
{
	if (!AABB(widget->rect.renderRect(), m_root->rect.renderRect()))
		return;

	if (!widget->visible) return;

	realizeStyle(*widget);

	glm::vec4 prevClip = parentClip;
	glm::vec4 currentClip = parentClip;

	glm::vec4 rect = TO_GL_COORDS(widget->rect.renderRect(), m_root->rect.size.y);

	float left	 = std::max(rect.x, parentClip.x);
	float top	 = std::max(rect.y, parentClip.y);
	float right	 = std::min(rect.x + rect.z, parentClip.x + parentClip.z);
	float bottom = std::min(rect.y + rect.w, parentClip.y + parentClip.w);

	float width	 = right - left;
	float height = bottom - top;

	if (width <= 0 || height <= 0)
		return; // fully clipped

	currentClip = glm::vec4(left, top, width, height);

	Canvas2D::setClipping(currentClip);
	Canvas2D::setDepth(submissionIndex++);

	for (auto& child : widget->children())
	{
		drawRecursive(child.get(), currentClip);
	}
	Canvas2D::setClipping(prevClip);
}

void UIService::updateRecursive(UIWidget* m_root, const UIRect& parentRect, ILayoutStrategy& strategy, int index)
{
	m_root->rect = strategy.layout(*m_root, parentRect, index);
	// This is pointer to base class, since there are different concrete implementations
	// this may hit performance because of virtual dispatch, be sure to profile it!
	auto& layoutStrategy = styles[m_root->style]->layoutStrategy;

	m_root->dirtyUpdate = false;

	int childIndex = 0;
	for (auto& child : m_root->children())
	{
		updateRecursive(child.get(), m_root->rect, *layoutStrategy, childIndex);
		childIndex++;
	}
}

void UIService::topWidgetAtPos(UIWidget* widget, glm::vec2 pos)
{
	for(auto& child : widget->children())
	{
		topWidgetAtPos(child.get(), pos);
	}

	if (isBlocked)
		return;

	if(widget->blocking && AABB(widget->rect.renderRect(), glm::vec4(pos, 1, 1)))
	{
		focused = widget;
		isBlocked = true;
	}

	return;
}

void UIService::destroy(const std::string& handle)
{
	if(!handles.contains(handle))
		return;

	handles[handle]->parent.lock()->eraseChild(handles[handle]);
	prevFocused = nullptr;

	destroy(handles[handle]);
}

void UIService::destroy(const std::shared_ptr<UIWidget>& widget)
{
	for (auto& child : widget->children())
		destroy(child);

	idPool.releaseId(widget->id);

	widget->clearChildren();
	widget->parent.reset();

	ids.erase(widget->id);
	if(handles.contains(widget->handle))
		handles.erase(widget->handle);
}

void UIService::destroy(const UIWidget* widget)
{
	destroy(widget);
}

UIWidget* UIService::widget(int id) const
{
	if (ids.contains(id))
	{
		return ids.at(id).get();
	}

	ServiceLocator::get<ILogger>()->error(std::format("Tried to extract ui widget by non-existent ID ({})!", id).c_str());
	return nullptr;
}

UIWidget* UIService::widget(std::string handle) const
{
	if (handles.contains(handle))
	{
		return handles.at(handle).get();
	}

	ServiceLocator::get<ILogger>()->error(std::format("Tried to extract ui widget by non-existent handle ({})!", handle).c_str());
	return nullptr;
}