#pragma once
#include "core/game.h"
#include "ecs/components.h"
#include "graphics/renderer2d.h"
#include "lib/FastNoiseLite.h"
#include "utility/file_util.h"
#include "core/resources.h"

#include <glad/glad.h>
#include "glm/glm.hpp"

#include <memory>
#include <iostream>
#include <vector>

using namespace glm;
using namespace core;

void Game::init(AppState& state)
{
	Renderer2D::Init();

	mainCam = std::make_unique<Camera>((float)viewportWidth, (float)viewportHeight);

	Resources::loadTexture(file_util::getPath("assets", "default.png").string(), "default");

	world = flecs::world();
}

void Game::update(float dt)
{

}

void Game::input(SDL_Event& e, float dt)
{
	
}

void Game::draw()
{
	glViewport(0, 0, viewportWidth, viewportHeight); //Clear entire viewport first
	glClearColor(0.025f, 0.025f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Renderer2D::drawText("Hello World!", vec2(500, 100));

	Renderer2D::drawTexture("default", ecs::Transform2D { 
		.position = vec3(400, 300, 1) 
	});

	Renderer2D::drawRect(vec2(800, 450), vec2(200, 200), 2.0f, SDL_Color{ 255, 0, 0, 255 });
	Renderer2D::render(mainCam.get());
}

void Game::quit()
{
	Renderer2D::destroy();
}