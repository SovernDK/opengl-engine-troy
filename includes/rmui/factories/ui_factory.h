#pragma once
#include <memory>
#include <functional>
#include <string>

#include "glm/glm.hpp"
#include "rmui/ui.h"

#include "rmui/ui_widget.h"
	
class IUIService;
class UIService;

template<typename TDerived>
class IWidgetFactory
{
protected:
	std::string m_style = "defButton";

	IUIService& service;

	UIRect localRect{ 0 };
	glm::vec2 pivot{ 0 };
	TexID bgTexture = 0;
	std::shared_ptr<UIWidget> m_parent;

	TDerived& self() { return static_cast<TDerived&>(*this); }
public:
	IWidgetFactory(IUIService& service) : service(service) {};
	virtual ~IWidgetFactory() = default;

	TDerived& setLocalRect(float x, float y, float w, float h) { localRect = UIRect(x, y, w, h); return self(); };
	TDerived& setLocPos(float x, float y) { localRect.pos = glm::vec2(x, y); return self(); };
	TDerived& setLocPos(UIAnchor anchor) { localRect.pos = uiAnchorToVec2(anchor); return self(); };
	TDerived& setLocSize(float w, float h) { localRect.size = glm::vec2(w, h); return self(); };

	TDerived& setPivot(UIAnchor anchor) { pivot = uiAnchorToVec2(anchor); return self(); };

	TDerived& setParent(const std::shared_ptr<UIWidget>& parent)
	{
		m_parent = parent;
		return self();
	}

	TDerived& setBG(const std::string& handle) { bgTexture = Resources::texture(handle)->id; return self(); };
	TDerived& setBG(TexID textureId) { bgTexture = textureId; return self(); };
	TDerived& setBG(GLuint textureId) { bgTexture = TexID{ textureId }; return self(); };

	TDerived& setStyle(const std::string style) { m_style = style; return self(); };
};

class UILabelFactory : public IWidgetFactory<UILabelFactory>
{
private:
	std::string m_text;
	TextAlign m_align = TextAlign::Left;
	TextVertAlign m_valign = TextVertAlign::Top;
public:
	UILabelFactory(IUIService& service) : IWidgetFactory(service) { m_style = "defText"; };

	std::shared_ptr<UIWidget> build(const std::string& handle);

	UILabelFactory& setText(std::string text) { m_text = text; return *this; };
	UILabelFactory& setText(std::string& text) { m_text = text; return *this; };

	UILabelFactory& setAlign(TextAlign align) { m_align = align; return *this; };
	UILabelFactory& setValign(TextVertAlign valign) { m_valign = valign; return *this; };
};

class UIWindowFactory : public IWidgetFactory<UIWindowFactory>
{
public:
	UIWindowFactory(IUIService& service) : IWidgetFactory(service) { m_style = "topbar"; };

	std::shared_ptr<UIWidget> build(const std::string& handle);
};

class UIButtonFactory : public IWidgetFactory<UIButtonFactory>
{
private:
	std::function<void(UIWidget*)>            onClick;
	std::function<void(UIWidget*, glm::vec2)> onPressed;
	std::function<void(UIWidget*)>            onEnterHover;
	std::function<void(UIWidget*)>            onExitHover;

	TexID m_image = 0;
	bool m_keepAspectRatio = false;
	std::string m_text;
	TextAlign m_align = TextAlign::Left;
	TextVertAlign m_valign = TextVertAlign::Top;
public:
	UIButtonFactory(IUIService& service) : IWidgetFactory(service) { m_style = "defButton"; }

	UIButtonFactory& addOnClick(const std::function<void(UIWidget*)>& cb) { onClick = cb; return *this; }
	UIButtonFactory& addOnPressed(const std::function<void(UIWidget*, glm::vec2)>& cb) { onPressed = cb; return *this; }
	UIButtonFactory& addOnEnterHover(const std::function<void(UIWidget*)>& cb) { onEnterHover = cb; return *this; }
	UIButtonFactory& addOnExitHover(const std::function<void(UIWidget*)>& cb) { onExitHover = cb; return *this; }

	UIButtonFactory& setImage(TexID imageId) { m_image = imageId; return *this; };
	UIButtonFactory& setText(std::string text) { m_text = text; return *this; };
	UIButtonFactory& setText(std::string& text) { m_text = text; return *this; };

	UIButtonFactory& setAlign(TextAlign align) { m_align = align; return *this; };
	UIButtonFactory& setValign(TextVertAlign valign	) { m_valign = valign; return *this; };
	
	UIButtonFactory& setKeepAspect(bool keepAspectRatio) { m_keepAspectRatio = keepAspectRatio; return *this; };

	std::shared_ptr<UIWidget> build(const std::string& handle);
};