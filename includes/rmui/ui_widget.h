#pragma once
#include "rmui/ui.h"

#include "graphics/material.h"
#include "ecs/base_components.h"
#include "rmui/style/ui_style.h"

#include <memory>
#include <iostream>
#include <canvas_2d.h>

class UIInteraction;
struct UIComponent;

class UIWidget
{
protected:
	std::vector<std::shared_ptr<UIWidget>> m_children;
public:
	int id = 0;
	bool dirtyUpdate = false;

	UIRect rect{ 0 };
	UIRect localRect{ 1 };

	glm::vec2 pivot = glm::vec2(0.0f);

	bool visible = true;
	bool blocking = true;
	bool interactive = false;

	std::string style = "default";
	std::string handle = "";

	std::weak_ptr<UIWidget> parent;
	std::unique_ptr<UIInteraction> interaction;
	std::vector<std::pair<std::type_index, std::unique_ptr<UIComponent>>> components;
private:
	ecs::Sprite m_sprite;
	ecs::Transform2D m_transform;
public:
	UIWidget(int id) : id(id)
	{
		m_sprite.material = Resources::sharedMat("ui");
		m_sprite.material.blendMode = BlendMode::Alpha;

		interaction = std::make_unique<UIInteraction>();
	};

	UIWidget(const UIWidget& other) = delete;
	UIWidget& operator=(const UIWidget& other) = delete;

	virtual ~UIWidget()
	{
		m_children.clear();
		parent.reset();
		components.clear();
	}

	const auto& children() const { return m_children; }
	void clearChildren() { m_children.clear(); }
	void addChild(std::shared_ptr<UIWidget> child) { m_children.push_back(child); }
	void eraseChild(std::shared_ptr<UIWidget> child)
	{
		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it != m_children.end())
		{
			m_children.erase(it);
		}
	}

	void setLocalRect(UIRect rect) { localRect = rect; setDirty(); }

	void setLocalPosition(glm::vec2 pos) { localRect.pos = pos; setDirty(); }
	void setLocalPosition(float x, float y) { localRect.pos = glm::vec2(x, y); setDirty(); }
	void setLocalPosition(UIAnchor pos) { localRect.pos = uiAnchorToVec2(pos); setDirty(); }

	void setLocalSize(glm::vec2 size) { localRect.size = size; setDirty(); }
	void setLocalSize(float width, float height) { localRect.size = glm::vec2(width, height); setDirty(); }
	void setLocalSize(float size) { localRect.size = glm::vec2(size, size); setDirty(); }

	void setPivot(glm::vec2 p) { pivot = p; /*setDirty();*/ }
	void setPivot(UIAnchor p) { pivot = uiAnchorToVec2(p);/* setDirty();*/ }

	void setDirty()
	{
		dirtyUpdate = true;
		if (!parent.expired())
		{
			parent.lock()->setDirty();
		}
	}

	ecs::Sprite& sprite()
	{
		m_sprite.size = rect.size;

		return m_sprite;
	}

	ecs::Transform2D& transform()
	{
		m_transform.position = glm::vec3(rect.pos, 1.0f);
		return m_transform;
	}

	MaterialInstance& material() { return m_sprite.material; }

	template<typename TComponent, typename... TArgs>
	void addComponent(TArgs&&... args)
	{
		components.push_back({ typeid(TComponent), std::make_unique<TComponent>(std::forward<TArgs>(args)...) });
		std::sort(components.begin(), components.end(), [](const auto& a, const auto& b) {
			return a.second->priority < b.second->priority;
		});
	}

	template<typename TComponent>
	TComponent* tryGetComponent()
	{
		for (auto& comp : components)
		{
			if (comp.first == typeid(TComponent))
				return static_cast<TComponent*>(comp.second.get());
		}

		return nullptr;
	}
};


class UIInteraction
{
private:
	std::vector<std::function<void(UIWidget*)>> onClickFuncs;
	std::vector<std::function<void(UIWidget*, glm::vec2)>> onPressedFuncs;
	std::vector<std::function<void(UIWidget*)>> onEnterHoverFuncs;
	std::vector<std::function<void(UIWidget*)>> onExitHoverFuncs;
public:
	bool hovered = false;
public:
	UIInteraction() = default;
	UIInteraction(const UIInteraction& other) = default;
	UIInteraction& operator=(const UIInteraction& other) = default;

	~UIInteraction() = default;

	void addOnClick(const std::function<void(UIWidget*)>& callback) { onClickFuncs.push_back(callback); }
	void addOnPressed(const std::function<void(UIWidget*, glm::vec2)>& callback) { onPressedFuncs.push_back(callback); }
	void addOnEnterHover(const std::function<void(UIWidget*)>& callback) { onEnterHoverFuncs.push_back(callback); }
	void addOnExitHover(const std::function<void(UIWidget*)>& callback) { onExitHoverFuncs.push_back(callback); }

	void triggerOnClick(UIWidget* widget)
	{
		for (auto& func : onClickFuncs)
		{
			func(widget);
		}
	}

	void triggerOnPressed(UIWidget* widget, glm::vec2 mousePos)
	{
		for (auto& func : onPressedFuncs)
		{
			func(widget, mousePos);
		}
	}

	void triggerOnEnterHover(UIWidget* widget)
	{
		for (auto& func : onEnterHoverFuncs)
		{
			func(widget);
		}

		hovered = true;
	}

	void triggerOnExitHover(UIWidget* widget)
	{
		for (auto& func : onExitHoverFuncs)
		{
			func(widget);
		}

		hovered = false;
	}
};

#pragma region Components
struct UIComponent
{
	int priority = 0;

	UIComponent(int priority = 0) : priority(priority) {};
	virtual ~UIComponent() = default;

	virtual void update(UIWidget* widget) = 0;
	virtual void realize(UIWidget* widget, const StyleComponent& style) = 0;
};

struct UIText : public UIComponent
{
	std::string text = "";

	TextAlign align		 = TextAlign::Left;
	TextVertAlign valign = TextVertAlign::Middle;

	glm::vec2 textOrigin = glm::vec2(0.0f);

	UIText() : UIComponent(2) {};
	UIText(std::string text) : UIComponent(2) {
		this->text = text;
	};

	UIText(std::string text, TextAlign align) : UIComponent(2) 
	{
		this->text = text;
		this->align = align;
	};

	UIText(std::string text, TextAlign align, TextVertAlign valign) : UIComponent(2) 
	{
		this->text = text;
		this->align = align;
		this->valign = valign;
	};

	void update(UIWidget* widget) override
	{

	}

	void realize(UIWidget* widget, const StyleComponent& style) override
	{
		int fontSize = style.fontSize;
		auto& _text = text;
		auto fontColor = style.fontColor;

		if(widget->interaction->hovered)
			fontColor = style.fontHoverColor;

		glm::vec2 tSize = Canvas2D::textSize(_text, style.fontName, fontSize);
		glm::vec2 tOrigin = Canvas2D::textOrigin(_text, style.fontName, fontSize);

		if (widget->rect.size.x < tSize.x)
		{
			float bigger = tSize.x;
			float lesser = widget->rect.size.x;

			// Shrink font size
			//fontSize *= lesser / bigger;

			// add ... if text is too long
			_text = _text.substr(0, static_cast<size_t>(_text.size() * (lesser / bigger)));
			_text += "...";
		}

		switch (align)
		{
		case TextAlign::Left:
			textOrigin.x = widget->transform().position.x;
			break;
		case TextAlign::Middle:
			textOrigin.x = widget->transform().position.x + (widget->rect.size.x - tSize.x) * 0.5f;
			break;
		case TextAlign::Right:
			textOrigin.x = widget->transform().position.x + (widget->rect.size.x - tSize.x);
			break;
		}

		switch (valign)
		{
		case TextVertAlign::Top:
			textOrigin.y = widget->transform().position.y + tSize.y;
			break;
		case TextVertAlign::Middle:
			textOrigin.y = widget->transform().position.y + (widget->rect.size.y + tSize.y) * 0.5f;
			break;
		case TextVertAlign::Bottom:
			textOrigin.y = widget->transform().position.y + widget->rect.size.y - tOrigin.y;
			break;
		}

		ecs::Transform2D textTransform = transform();
		Canvas2D::drawText(_text, textTransform, style.fontName, fontSize, fontColor);
	}

	ecs::Transform2D transform()
	{
		return ecs::Transform2D{ .position = glm::vec3(textOrigin, 1.0f) };
	}
};

struct UIBackground : public UIComponent
{
	TexID texture = 0;
	bool keepAspectRatio = false;

	UIBackground() : UIComponent(1) {};
	UIBackground(TexID _texture, bool _keepAspectRatio) : UIComponent(1)
	{
		texture = _texture;
		keepAspectRatio = _keepAspectRatio;
	};

	void update(UIWidget* widget) override
	{

	}

	void realize(UIWidget* widget, const StyleComponent& style) override
	{
		widget->material().setTexture("image", texture);
		if(texture.id == 0)
			widget->material().setProperty("useTexture", false);
		else
			widget->material().setProperty("useTexture", true);

		Canvas2D::drawSprite(widget->sprite(), widget->transform());
	}
};

struct UIDropShadow : public UIComponent
{
	UIDropShadow() : UIComponent(0) {};

	void update(UIWidget* widget) override
	{

	}

	void realize(UIWidget* widget, const StyleComponent& style) override
	{
		if (!style.dropShadow) return;

		auto material = Canvas2D::loadToArena<MaterialInstance>(Resources::sharedMat("ui"));
		material->setProperty("mainColor", glm::vec4(glm::vec3(0.0f), 1.0f));
		material->setProperty("useTexture", false);

		glm::vec4 shadowRect = widget->rect.renderRect() + glm::vec4(style.shadowOffset, 0.0f, 0.0f);
		Canvas2D::drawQuad(shadowRect, material);
	}
};
#pragma endregion Components

#pragma region Widget concrete classes
class UIButton : public UIWidget
{
public:
	UIWidget* label = nullptr;
public:
	UIButton(int id) : UIWidget(id) {};
};

#pragma endregion