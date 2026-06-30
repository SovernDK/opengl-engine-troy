#pragma once
#include "editor/editor.h"
#include "lib/imgui/imgui.h"
#include "SDL3/SDL_timer.h"

#include "core/game.h"
#include "core/profiler.h"

#include "services/service_locator.h"
#include "services/ui_service.h"

#include <memory>

using namespace editor;

void Editor::update(float dt)
{
	core::Game& game = dynamic_cast<core::Game&>(m_ctx);

	float toolbarWidth = game.screenWidth - game.viewportWidth;
	glm::vec2 toolbarPos = glm::vec2(game.viewportWidth, 0);
	toolbar(toolbarWidth, game.viewportWidth, toolbarPos);

	generalDebugWindow(core::Profiler::instance().getFPS(),
		core::Profiler::instance().getDeltaTime(),
		core::Profiler::instance().getDrawCalls(),
		core::Profiler::instance().getMemoryStats().workingSetMB,
		core::Profiler::instance().getMemoryStats().privateBytesMB,
		core::Profiler::instance().frameBufferUsed,
		core::Profiler::instance().frameBufferCap);

	cameraInspector(*game.mainCam.get());

	//stateEditor(game.gameFlowFsm->getCurrent().lock()->name());
	sceneEditor();

	worldTree(*game.world.get());
	uiTree();

	entityInspector(*game.world.get());
	uiInspector();

	ImGui::Begin("Toolbar");
	if (ImGui::CollapsingHeader("World map"))
	{
		worldMap->render();
	}
	ImGui::End();
}

void Editor::toolbar(int width, int height, glm::vec2 pos)
{
	ImGuiWindowFlags toolbar_flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2((float)width, (float)height), ImGuiCond_Always);

	ImGui::Begin("Toolbar", nullptr, toolbar_flags);
	ImGui::End();
}

void Editor::generalDebugWindow(Uint64 fps, float dt, int drawCalls, float memMBWorking, float memMBPrivate, size_t frameMemUsed, size_t frameMemCap)
{
	ImGui::Begin("Toolbar");
	if (ImGui::CollapsingHeader("Debug"))
	{
		ImGui::Text("Elapsed time: %.2fs", SDL_GetTicks() / 1000.0f);
		ImGui::Text("Delta time: %f", dt);
		ImGui::Text("FPS: %i", fps);
		ImGui::Text("Draw calls: %i", drawCalls);
		ImGui::SeparatorText("Memory");
		ImGui::Text("Working mem: %.2f MB", memMBWorking);
		ImGui::Text("Mem usage: %.2f MB", memMBPrivate);
		ImGui::Text("Frame buffer: %d / %d kB", frameMemUsed / 1024, frameMemCap / 1024);
	}
	ImGui::End();
}

void Editor::sceneEditor()
{
	ImGui::Begin("Toolbar");
	if (ImGui::CollapsingHeader("Scenes"))
	{
		ImGui::SeparatorText("Active scenes:");
		for (auto& scene : ServiceLocator::get<ISceneService>()->activeScenes())
		{
			ImGui::Text(scene->name().c_str());
		}
	}
	ImGui::End();
}

void Editor::stateEditor(std::string currentState)
{
	ImGui::Begin("Toolbar");
	if (ImGui::CollapsingHeader("Game Flow FSM"))
	{
		ImGui::SeparatorText("Current state");
		ImGui::Text(currentState.c_str());
	}
	ImGui::End();
}

void Editor::cameraInspector(ICamera& camera)
{
	ImGui::Begin("Toolbar");
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::SeparatorText("Background Color");
		SDL_Color& bg = camera.backgroundColor;
		float color[4] = { bg.r / 255.0f, bg.g / 255.0f, bg.b / 255.0f, bg.a / 255.0f };
		if (ImGui::ColorEdit4("Background", color))
		{
			bg.r = (Uint8)(color[0] * 255.0f);
			bg.g = (Uint8)(color[1] * 255.0f);
			bg.b = (Uint8)(color[2] * 255.0f);
			bg.a = (Uint8)(color[3] * 255.0f);
		}

		ImGui::SeparatorText("Read-only");
		glm::vec4 vp = camera.viewport();
		ImGui::Text("Viewport: %.1f, %.1f, %.1f, %.1f", vp.x, vp.y, vp.z, vp.w);

		glm::vec4 b = camera.bounds();
		ImGui::Text("Bounds: %.2f, %.2f, %.2f, %.2f", b.x, b.y, b.z, b.w);

		glm::mat4 view = camera.view();
		ImGui::Text("View Matrix:");
		for (int r = 0; r < 4; ++r)
			ImGui::Text("%.2f %.2f %.2f %.2f", view[0][r], view[1][r], view[2][r], view[3][r]);

		// ---- Mutators via interface methods ----
		ImGui::SeparatorText("Move");
		static glm::vec3 moveDelta(0.0f);
		ImGui::DragFloat3("Move delta", &moveDelta.x, 0.1f);
		if (ImGui::Button("Apply Move"))
			camera.move(moveDelta);

		ImGui::SeparatorText("Zoom");
		static float zoomDelta = 0.0f;
		ImGui::DragFloat("Zoom delta", &zoomDelta, 0.01f);
		if (ImGui::Button("Apply Zoom"))
			camera.changeZoom(zoomDelta);

		ImGui::SeparatorText("Size");
		static float w = 800, h = 600;
		ImGui::DragFloat("Resize width", &w);
		ImGui::DragFloat("Resize height", &h);
		if (ImGui::Button("Apply Resize"))
			camera.resize(w, h);
	}								
	ImGui::End();
}

void Editor::worldTree(ecs::ECSWorld& world)
{
	ImGui::Begin("Toolbar");
	if (ImGui::CollapsingHeader("Entities##"))
	{
		world.each([&](ecs::Entity& e)
		{
			std::string label = std::format("Entity({}) ", e.id);
			bool isSelected = (selectedId == e.id);

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanAvailWidth;
			if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

			if (ImGui::TreeNodeEx(label.c_str(), flags, label.c_str()))
			{
				if (ImGui::IsItemClicked())
					selectedId = e.id;

				ImGui::TreePop();
			}
		});
	}
	ImGui::End();
}

void Editor::entityInspector(ecs::ECSWorld& world)
{
	if (selectedId <= 0) 
	{
		return;
	}

	core::Game& game = dynamic_cast<core::Game&>(m_ctx);
	ecs::Entity& e = game.world->entity(selectedId);
	
	ImGui::Begin("Toolbar");
	if (ImGui::CollapsingHeader("Entity Inspector"))
	{
		for (auto& type : world.types(e.id))
		{
			if (auto transform = world.isType<ecs::Transform2D>(type))
			{
				ImGui::Text(typeid(*transform).name());
				ImGui::InputFloat("X: ", &transform->position.x);
				ImGui::InputFloat("Y: ", &transform->position.y);
				ImGui::InputFloat("Z: ", &transform->position.z);
			}
			else
			{
				ImGui::Text(type.first.name());
			}
		}
	}
	ImGui::End();
}

void Editor::uiTree()
{
	ImGui::Begin("Toolbar");
	if (ImGui::CollapsingHeader("UI Widgets##"))
	{
		uiNode(ServiceLocator::get<IUIService>()->root());
	}
	ImGui::End();
}

void Editor::uiNode(UIWidget& widget)
{
	std::string label = std::format("widget_{}", widget.id);
	bool isSelected = (uiSelectedId == widget.id);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth;

	if (widget.children().empty())
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

	if (ImGui::TreeNodeEx(label.c_str(), flags, label.c_str()))
	{
		if (ImGui::IsItemClicked())
			uiSelectedId = widget.id;

		for (auto& child : widget.children())
		{
			uiNode(*child.get());
		}

		ImGui::TreePop();
	}
}

void Editor::uiInspector()
{
	if (uiSelectedId <= 0)
	{
		return;
	}

	ImGui::Begin("Toolbar");
	if (ImGui::CollapsingHeader("UI Inspector"))
	{
		auto selectedWidget = ServiceLocator::get<IUIService>()->widget(uiSelectedId);
		ImGui::Text(typeid(*selectedWidget).name());
		ImGui::Separator();
		ImGui::Text("World Rect");
		ImGui::Text(std::format("Pos: x({}), y({})",	selectedWidget->rect.pos.x,		selectedWidget->rect.pos.y).c_str());
		ImGui::Text(std::format("Size: x({}), y({})",	selectedWidget->rect.size.x,	selectedWidget->rect.size.y).c_str());
		ImGui::Text("Local Rect");
		ImGui::Text(std::format("Pos: x({}), y({})",	selectedWidget->localRect.pos.x,	selectedWidget->localRect.pos.y).c_str());
		ImGui::Text(std::format("Size: x({}), y({})",	selectedWidget->localRect.size.x,	selectedWidget->localRect.size.y).c_str());
		ImGui::Text("States");
		ImGui::Text(std::format("Visible: {}",		selectedWidget->visible).c_str());
		ImGui::Text(std::format("Interactive: {}",	selectedWidget->interactive).c_str());
		ImGui::Text("Style");
		ImGui::Text(std::format("Style: {}", selectedWidget->style).c_str());
	}
	ImGui::End();
}