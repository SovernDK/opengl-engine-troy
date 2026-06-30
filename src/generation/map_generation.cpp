#pragma once
#include "generation/map_generation.h"
#include <utility/file_util.h>
#include "utility/utils.h"
#include "resources.h"

using namespace std;

MapGeneration::MapGeneration()
{
	heightSetting.SetSeed(1337);
	heightSetting.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	heightSetting.SetFrequency(0.001f);

	heightSetting.SetFractalType(FastNoiseLite::FractalType_FBm);
	heightSetting.SetFractalOctaves(6);
	heightSetting.SetFractalLacunarity(2.0f);
	heightSetting.SetFractalGain(0.5f);

	mountainsSetting.SetSeed(1337);
	mountainsSetting.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	mountainsSetting.SetFrequency(0.001f);

	mountainsSetting.SetFractalType(FastNoiseLite::FractalType_FBm);
	mountainsSetting.SetFractalOctaves(8);
	mountainsSetting.SetFractalLacunarity(2.0f);
	mountainsSetting.SetFractalGain(0.5f);

	moistureSetting.SetSeed(1337);
	moistureSetting.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	moistureSetting.SetFrequency(0.2f);

	moistureSetting.SetFractalType(FastNoiseLite::FractalType_FBm);
	moistureSetting.SetFractalOctaves(6);
	moistureSetting.SetFractalLacunarity(2.0f);
	moistureSetting.SetFractalGain(0.5f);

	/*warpSetting.SetSeed(1337);
	warpSetting.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	warpSetting.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
	warpSetting.SetDomainWarpAmp(14.0f);
	warpSetting.SetFrequency(0.035f);*/

	warpXSetting.SetSeed(1337);
	warpXSetting.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	warpXSetting.SetFrequency(0.02f);

	warpXSetting.SetFractalType(FastNoiseLite::FractalType_FBm);
	warpXSetting.SetFractalOctaves(6);
	warpXSetting.SetFractalLacunarity(2.0f);
	warpXSetting.SetFractalGain(0.5f);

	warpYSetting.SetSeed(625);
	warpYSetting.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	warpYSetting.SetFrequency(0.001f);

	warpYSetting.SetFractalType(FastNoiseLite::FractalType_FBm);
	warpYSetting.SetFractalOctaves(6);
	warpYSetting.SetFractalLacunarity(2.0f);
	warpYSetting.SetFractalGain(0.001f);
}

void MapGeneration::generate(const int width, const int height, const int seed)
{
	assert(height > 0 && width > 0);

	heightPixels.clear();
	normalPixels.clear();
	moisturePixels.clear();

	m_width = width;
	m_height = height;

	auto heightMap = generateHeightMap(m_width, m_height, seed);
	heightPixels.resize(m_width * m_height * 4);
	generateTextureHigh(heightPixels, heightMap);

	normalPixels.resize(m_width * m_height * 4);
	generateNormal(normalPixels, heightMap);

	auto moistureMap = generateMoistureMap(m_width, m_height, seed);
	moisturePixels.resize(m_width * m_height * 4);
	generateTexture(moisturePixels, moistureMap);
}

void MapGeneration::generateTexture(std::vector<uint8_t>& pixels, const std::vector<std::vector<float>>& noise)
{
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			int i = (y * m_width + x) * 4;
			float h = utils::normalize(noise.at(y)[x], -1.0f, 1.0f);
			pixels[i + 0] = (uint8_t)(h * 255);
			pixels[i + 1] = (uint8_t)(h * 255);
			pixels[i + 2] = (uint8_t)(h * 255);
			pixels[i + 3] = 255;
		}
	}
}

void MapGeneration::generateTextureHigh(std::vector<uint8_t>& pixels, const std::vector<std::vector<float>>& noise)
{
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			int i = (y * m_width + x) * 4;
			float h = utils::normalize(noise.at(y)[x], -1.0f, 1.0f);
			utils::packFloatToRGBA(h, &pixels[i]);
		}
	}
}

void MapGeneration::generateNormal(std::vector<uint8_t>& pixels, const std::vector<std::vector<float>>& noise)
{
	float strength = 25;

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			float hL = (x - 1 >= 0) ?		noise.at(y)[x - 1] : noise.at(y)[x];
			float hR = (x + 1 < m_width) ?	noise.at(y)[x + 1] : noise.at(y)[x];
			float hD = (y - 1 >= 0) ?		noise.at(y - 1)[x] : noise.at(y)[x];
			float hU = (y + 1 < m_height) ?	noise.at(y + 1)[x] : noise.at(y)[x];

			float dx = (hR - hL) * strength;
			float dy = (hU - hD) * strength;

			glm::vec3 normal = glm::vec3(dx, dy, 1.0f);
			normal = glm::normalize(normal);

			int i = (y * m_width + x) * 4;
			pixels[i + 0] = (uint8_t)((normal.x * 0.5f + 0.5f) * 255);
			pixels[i + 1] = (uint8_t)((normal.y * 0.5f + 0.5f) * 255);
			pixels[i + 2] = (uint8_t)((normal.z * 0.5f + 0.5f) * 255);
			pixels[i + 3] = 255;
		}
	}
}

vector<vector<float>> MapGeneration::generateHeightMap(const int width, const int height, const int seed)
{
    vector<vector<float>> map(height, vector<float>(width));
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
			float posX = (float)x;
			float posY = (float)y;

			(map)[y][x] = mountainsSetting.GetNoise(posX, posY);
        }
    }
    return map;
}

std::vector<std::vector<float>> MapGeneration::generateMountains(const int width, const int height, const int seed)
{
	vector<vector<float>> map(height, vector<float>(width));
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float posX = (float)x;
			float posY = (float)y;

			float strength = 14.0f;

			posX = warpXSetting.GetNoise((float)x, (float)y) * strength;
			posY = warpXSetting.GetNoise((float)x + 5.2f, (float)y + 1.3f) * strength;

			//warpSetting.DomainWarp(posX, posY);
			(map)[y][x] = heightSetting.GetNoise((float)x + posX, (float)y + posY);
		}
	}
	return map;
}

vector<vector<float>> MapGeneration::generateMoistureMap(const int width, const int height, const int seed)
{
    vector<vector<float>> map(height, vector<float>(width));
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            (map)[y][x] = moistureSetting.GetNoise((float)x, (float)y);
        }
    }
    return map;
}