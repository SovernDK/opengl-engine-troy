#pragma once
#include "rmui/ui.h"

#include "rmui/ui_interaction.h"
#include "graphics/material.h"

#include <memory>
#include <iostream>

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

	TexID textureId = 1;
	bool useTexture = false;
	bool keepAspectRatio = false;

	bool hasText = false;
	std::string text;

	std::weak_ptr<UIWidget> parent;

	std::unique_ptr<MaterialInstance> matInst	 = nullptr;
	std::unique_ptr<MaterialInstance> shadowInst = nullptr;

	std::unique_ptr<UIInteraction> interaction = nullptr;
public:
	UIWidget() {};
	UIWidget(int id) : id(id) {};

	UIWidget(const UIWidget& other) = delete;
	UIWidget& operator=(const UIWidget& other) = delete;
	
	virtual ~UIWidget() 
	{
		m_children.clear();
		parent.reset();
		matInst.reset();
		shadowInst.reset();
	}

	const auto& children() const { return m_children; }
	void clearChildren() { m_children.clear(); }
	void addChild(std::shared_ptr<UIWidget> child) { m_children.push_back(child); }
	void eraseChild(std::shared_ptr<UIWidget> child) { 
		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it != m_children.end()) { 
			m_children.erase(it);
		}
	}

	void setLocalRect(UIRect rect) { localRect = rect; setDirty(); }

	void setLocalPosition(glm::vec2 pos) { localRect.pos = pos; setDirty(); }
	void setLocalPosition(float x, float y) { localRect.pos = glm::vec2(x, y); setDirty(); }
	void setLocalPosition(UIAnchor pos)	{ localRect.pos = uiAnchorToVec2(pos); setDirty(); }

	void setLocalSize(glm::vec2 size) { localRect.size = size; setDirty(); }
	void setLocalSize(float width, float height) { localRect.size = glm::vec2(width, height); setDirty(); }
	void setLocalSize(float size) { localRect.size = glm::vec2(size, size); setDirty(); }

	void setPivot(glm::vec2 p) { pivot = p; setDirty(); }
	void setPivot(UIAnchor p) { pivot = uiAnchorToVec2(p); setDirty(); }

	void setDirty() { 
		dirtyUpdate = true;
		if (!parent.expired())
		{
			parent.lock()->setDirty();
		}
	}
};