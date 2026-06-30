#pragma once
#include "graphics/rendering/render_queue.h"

#include <algorithm>

void RenderQueue::submit(RenderCommand cmd)
{
	buffer.push_back(cmd);
}

void RenderQueue::sort()
{
	std::sort(buffer.begin(), buffer.end(),
		[](const RenderCommand& a, const RenderCommand b)
		{
			return a.depth < b.depth;
		});
}

void RenderQueue::clear()
{
	buffer.clear();
}