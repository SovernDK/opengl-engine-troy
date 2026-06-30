#pragma once
#include "rmui/ui.h"
#include "rmui/ui_widget.h"
#include "services/service.h"
#include "utility/id_pool.h"

#include "rmui/factories/ui_factory.h"

#include <unordered_map>
#include <string>
#include <memory>

#include <nlohmann/json.hpp>
#include <rmui/ui_layout.h>
#include <graphics/graphics.h>

union SDL_Event;
class Material;

struct StyleComponent
{
	std::string name;

	SDL_Color bgColor;
	SDL_Color hoverColor;

	std::unique_ptr<ILayoutStrategy> layoutStrategy;

	Texture2D* texture;

	bool dropShadow = false;
	glm::vec2 shadowOffset = glm::vec2(0);
};

class IUIService : public IService
{
public:
	virtual ~IUIService() = default;

	virtual void loadStyles(nlohmann::json data) = 0;

	virtual void resizeCanvas(int width, int height) = 0;
	virtual void draw() = 0;
	virtual void update() = 0;

	virtual void handleMouse(glm::vec2 mousePos) = 0;
	virtual void handleInput(SDL_Event& e) = 0;

	virtual void registerMats(const std::shared_ptr<Material>& material, const std::shared_ptr<Material>& shadow) = 0;
	virtual void registerMats(Material material, Material shadow) = 0;

	virtual UIWidget* widget(int id) const = 0;
	virtual UIWidget* widget(std::string handle) const = 0;
	virtual UIWidget& root() = 0;
	virtual int nextId() = 0;

	virtual void destroy(const std::string& handle) = 0;
	virtual void destroy(const std::shared_ptr<UIWidget>& widget) = 0;
	virtual void destroy(const UIWidget* widget) = 0;

	template<typename T>
	std::shared_ptr<T> create(const std::string& handle, const std::string& style, const std::shared_ptr<UIWidget>& parent)
	{
		std::shared_ptr<UIWidget> widget = std::make_shared<T>(nextId());
		initWidget(widget, handle, style, parent);
		return std::dynamic_pointer_cast<T>(widget);
	}

	virtual UIButtonFactory createButton() = 0;
	virtual UIWindowFactory createWindow() = 0;
protected:
	virtual void initWidget(std::shared_ptr<UIWidget> widget, const std::string& handle, const std::string& style, const std::shared_ptr<UIWidget>& parent) = 0;
};

class UIService : public IUIService
{
private:
	std::unordered_map<int, std::shared_ptr<UIWidget>> ids;
	std::unordered_map<std::string, std::shared_ptr<UIWidget>> handles;

	std::shared_ptr<UIWidget> m_root = nullptr;
	UIWidget* focused = nullptr;
	UIWidget* prevFocused = nullptr;

	std::unique_ptr<MaterialInstance> materialInstance = nullptr;
	std::unique_ptr<MaterialInstance> shadowInstance   = nullptr;

	std::unordered_map<std::string, std::unique_ptr<StyleComponent>> styles;

	const int submissionStart = 90;
	int submissionIndex = submissionStart;
	bool isBlocked = false;

	IdPool<int> idPool;
	int canvasWidth = 0;
	int canvasHeight = 0;
public:
	UIService() = default;
	~UIService() = default;

	void loadStyles(nlohmann::json data) override;

	void resizeCanvas(int width, int height) override;
	void draw() override;
	void update() override;
	void handleMouse(glm::vec2 mousePos) override;
	void handleInput(SDL_Event& e) override;

	void registerMats(const std::shared_ptr<Material>& material, const std::shared_ptr<Material>& shadow) override;
	void registerMats(Material material, Material shadow) override;

	void destroy(const std::string& handle) override;
	void destroy(const std::shared_ptr<UIWidget>& widget) override;
	void destroy(const UIWidget* widget) override;

	UIWidget* widget(int id) const override;
	UIWidget* widget(std::string handle) const override;

	UIWidget& root() override { return *m_root.get(); }
	int nextId() override { return idPool.getId(); }

	UIButtonFactory createButton() override { return UIButtonFactory(*this); }
	UIWindowFactory createWindow() override { return UIWindowFactory(*this); }
private:
	void realizeStyle(UIWidget& widget);
	void drawRecursive(UIWidget* m_root, const glm::vec4 parentClip);
	void updateRecursive(UIWidget* m_root, const UIRect& parentRect, ILayoutStrategy& strategy, int index);
	void topWidgetAtPos(UIWidget* widget, glm::vec2 pos);

	void initWidget(std::shared_ptr<UIWidget> widget, const std::string& handle, const std::string& style, const std::shared_ptr<UIWidget>& parent) override
	{
		widget->matInst = std::make_unique<MaterialInstance>(*materialInstance);
		widget->shadowInst = std::make_unique<MaterialInstance>(*shadowInstance);
		widget->style = style;

		if (parent)
		{
			widget->parent = parent;
			parent->addChild(widget);
		}
		else
		{
			widget->parent = m_root;
			m_root->addChild(widget);
		}
			
		widget->handle = handle;
		handles[handle] = widget;
		ids[widget->id] = widget;
	}
};