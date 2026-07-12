#pragma once
#include "glm/glm.hpp"

class UIWidget;
struct UIRect;
class UILabel;
struct Margin;
struct StyleComponent;

struct ILayoutStrategy
{
	ILayoutStrategy() = default;
	ILayoutStrategy(const ILayoutStrategy&) = default;
	ILayoutStrategy& operator=(const ILayoutStrategy&) = default;

	virtual ~ILayoutStrategy() = default;

    virtual UIRect layout(UIWidget& self, const UIRect& parentRect, int index) = 0;
};

struct HorizontalLayout : ILayoutStrategy
{
    float spacing = 0;
    Margin margin{ 0 };

    bool expand = false;
    bool fit    = false;

    UIRect layout(UIWidget& self, const UIRect& parentRect, int index) override;
};

struct VerticalLayout : ILayoutStrategy
{
    float spacing = 0;
    Margin margin{ 0 };

    bool expand = false;
    bool fit    = false;

    UIRect layout(UIWidget& self, const UIRect& parentRect, int index) override;
};

struct AbsoluteLayout : ILayoutStrategy
{
    UIRect layout(UIWidget& self, const UIRect& parentRect, int index) override;
};