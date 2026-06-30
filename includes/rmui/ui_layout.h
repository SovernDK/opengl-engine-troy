#pragma once
#include "glm/glm.hpp"

class UIWidget;
struct UIRect;
struct Margin;

struct ILayoutStrategy
{
    virtual ~ILayoutStrategy() = default;
    virtual UIRect layout(UIWidget& self, const UIRect& parentRect, int index) = 0;
};
struct HorizontalLayout : ILayoutStrategy
{
    float spacing = 0;
    Margin margin{ 0 };

    bool expand = false;
    bool fit    = false;

    ~HorizontalLayout() = default;
    UIRect layout(UIWidget& self, const UIRect& parentRect, int index) override;
};
struct VerticalLayout : ILayoutStrategy
{
    float spacing = 0;
    Margin margin{ 0 };

    bool expand = false;
    bool fit    = false;

    ~VerticalLayout() = default;
    UIRect layout(UIWidget& self, const UIRect& parentRect, int index) override;
};
struct AbsoluteLayout : ILayoutStrategy
{
    ~AbsoluteLayout() = default;
    UIRect layout(UIWidget& self, const UIRect& parentRect, int index) override;
};