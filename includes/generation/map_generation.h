#pragma once
#include "lib/FastNoiseLite.h"

#include <vector>

class MapGeneration
{
public:
	FastNoiseLite heightSetting;
	FastNoiseLite mountainsSetting;
	FastNoiseLite riverSetting;
	FastNoiseLite moistureSetting;

	FastNoiseLite warpXSetting;
	FastNoiseLite warpYSetting;

	std::vector<uint8_t> heightPixels;
	std::vector<uint8_t> normalPixels;
	std::vector<uint8_t> moisturePixels;

	int m_width = 512;
	int m_height = 512;
public:
	MapGeneration();
	~MapGeneration() = default;

	void generate(const int width, const int height, const int seed);
	void generateTexture(std::vector<uint8_t>& pixels, const std::vector<std::vector<float>>& noise);
	void generateTextureHigh(std::vector<uint8_t>& pixels, const std::vector<std::vector<float>>& noise);
	void generateNormal(std::vector<uint8_t>& pixels, const std::vector<std::vector<float>>& noise);

	std::vector<std::vector<float>> generateHeightMap(const int width, const int height, const int seed);
	std::vector<std::vector<float>> generateMoistureMap(const int width, const int height, const int seed);
	std::vector<std::vector<float>> generateMountains(const int width, const int height, const int seed);
};