#pragma once
#include <memory>
#include <functional>
#include <string>

#include "glm/glm.hpp"
#include "rmui/ui.h"

#include "rmui/ui_button.h"
#include "rmui/ui_window.h"
#include "rmui/ui_widget.h"

class IUIService;
class UIService;

class IWidgetFactory
{
protected:
	std::string m_style = "defButton";

	IUIService& service;

	UIRect localRect{ 0 };
	glm::vec2 pivot{ 0 };
	std::shared_ptr<UIWidget> m_parent;
public:
	IWidgetFactory(IUIService& service) : service(service) {};
	virtual ~IWidgetFactory() = default;

	IWidgetFactory& setLocalRect(float x, float y, float w, float h) { localRect = UIRect(x, y, w, h); return *this; };
	IWidgetFactory& setLocPos(float x, float y) { localRect.pos = glm::vec2(x, y); return *this; };
	IWidgetFactory& setLocPos(UIAnchor anchor) { localRect.pos = uiAnchorToVec2(anchor); return *this; };
	IWidgetFactory& setLocSize(float w, float h) { localRect.size = glm::vec2(w, h); return *this; };

	IWidgetFactory& setPivot(UIAnchor anchor) { pivot = uiAnchorToVec2(anchor); return *this; };

	template<typename T>
	IWidgetFactory& setParent(const std::shared_ptr<T>& parent)
	{
		static_assert(std::is_base_of_v<UIWidget, T>, "T must derive from UIWidget");
		m_parent = std::static_pointer_cast<UIWidget>(parent);
		return *this;
	}

	IWidgetFactory& setStyle(const std::string style) { m_style = style; return *this; };
};

class UIWindowFactory : public IWidgetFactory
{
public:
	UIWindowFactory(IUIService& service) : IWidgetFactory(service) { m_style = "topbar"; };

	UIWindowFactory& setLocalRect(float x, float y, float w, float h) { IWidgetFactory::setLocalRect(x, y, w, h); return *this; }
	UIWindowFactory& setParent(const std::shared_ptr<UIWidget>& parent) { IWidgetFactory::setParent(parent); return *this; }

	template<typename T>
	UIWindowFactory& setParent(const std::shared_ptr<T>& parent) { IWidgetFactory::setParent(parent); return *this; }
	UIWindowFactory& setStyle(const std::string style) { IWidgetFactory::setStyle(style); return *this; };

	UIWindowFactory& setLocPos(float x, float y) { IWidgetFactory::setLocPos(x, y); return *this; };
	UIWindowFactory& setLocPos(UIAnchor anchor) { IWidgetFactory::setLocPos(anchor); return *this; };
	UIWindowFactory& setLocSize(float w, float h) { IWidgetFactory::setLocSize(w, h); return *this; };

	UIWindowFactory& setPivot(UIAnchor anchor) { IWidgetFactory::setPivot(anchor); return *this; };

	std::shared_ptr<UIWindow> build(const std::string& handle);
};

class UIButtonFactory : public IWidgetFactory
{
private:
	std::function<void(UIWidget*)>            onClick;
	std::function<void(UIWidget*, glm::vec2)> onPressed;
	std::function<void(UIWidget*)>            onEnterHover;
	std::function<void(UIWidget*)>            onExitHover;

	TexID m_image = 1;
	bool m_keepAspectRatio = false;
	std::string m_text;
public:
	UIButtonFactory(IUIService& service) : IWidgetFactory(service) { m_style = "defButton"; }

	UIButtonFactory& setLocalRect(float x, float y, float w, float h) { IWidgetFactory::setLocalRect(x, y, w, h); return *this; }
	UIButtonFactory& setParent(const std::shared_ptr<UIWidget>& parent) { IWidgetFactory::setParent(parent); return *this; }

	template<typename T>
	UIButtonFactory& setParent(const std::shared_ptr<T>& parent) { IWidgetFactory::setParent(parent); return *this; }
	UIButtonFactory& setStyle(const std::string style) { IWidgetFactory::setStyle(style); return *this; };

	UIButtonFactory& setOnClick(const std::function<void(UIWidget*)>& cb) { onClick = cb; return *this; }
	UIButtonFactory& setOnPressed(const std::function<void(UIWidget*, glm::vec2)>& cb) { onPressed = cb; return *this; }
	UIButtonFactory& setOnEnterHover(const std::function<void(UIWidget*)>& cb) { onEnterHover = cb; return *this; }
	UIButtonFactory& setOnExitHover(const std::function<void(UIWidget*)>& cb) { onExitHover = cb; return *this; }

	UIButtonFactory& setLocPos(float x, float y) { IWidgetFactory::setLocPos(x, y); return *this; };
	UIButtonFactory& setLocPos(UIAnchor anchor) { IWidgetFactory::setLocPos(anchor); return *this; };
	UIButtonFactory& setLocSize(float w, float h) { IWidgetFactory::setLocSize(w, h); return *this; };

	UIButtonFactory& setPivot(UIAnchor anchor) { IWidgetFactory::setPivot(anchor); return *this; };

	UIButtonFactory& setImage(TexID imageId) { m_image = imageId; return *this; };
	UIButtonFactory& setText(std::string text) { m_text = text; return *this; };
	UIButtonFactory& setText(std::string& text) { m_text = text; return *this; };
	
	UIButtonFactory& setKeepAspect(bool keepAspectRatio) { m_keepAspectRatio = keepAspectRatio; return *this; };

	std::shared_ptr<UIButton> build(const std::string& handle);
};