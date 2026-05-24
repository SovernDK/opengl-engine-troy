#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#include "SDL3/SDL.h"

namespace file_util
{
	namespace fs = std::filesystem;

	inline std::string load(const std::string& filePath)
	{
		if (!fs::exists(filePath))
		{
			std::cerr << "[FILE ERROR] File does not exist: " << filePath << "\n";
			return {};
		}

		std::ifstream file(filePath);

		if (!file.is_open())
		{
			std::cerr << "[FILE ERROR] Failed to open file: " << filePath << "\n";
			return {};
		}

		std::stringstream buffer;

		buffer << file.rdbuf();

		std::string content = buffer.str();

		if (content.empty())
		{
			std::cerr << "[FILE ERROR] File is empty: " << filePath << "\n";
			return {};
		}

		return buffer.str();
	}

	inline fs::path getBasePath()
	{
		static fs::path cached;

		if (cached.empty())
		{
			const char* base = SDL_GetBasePath();

			if (base)
				cached = base;
			else
				cached = fs::current_path();
		}

		return cached;
	}

	inline fs::path getPath(const std::string& relative)
	{
		const char* basePath = SDL_GetBasePath();
		if (!basePath)
		{
			return fs::current_path() / relative;
		}

		fs::path result = fs::path(basePath) / relative;
		return result;
	}

	template<typename... Args>
	inline fs::path getPath(Args&&... parts)
	{
		fs::path result = getBasePath();

		((result /= fs::path(std::forward<Args>(parts))), ...);

		return result;
	}

	inline std::string getNameFromPath(const std::string filepath, bool removeExtension = true)
	{
		return (removeExtension) ? fs::path(filepath).filename().stem().string() : fs::path(filepath).filename().string();
	}

	inline std::string getNameFromPath(const fs::path filepath, bool removeExtension = true)
	{
		return (removeExtension) ? filepath.filename().stem().string() : filepath.filename().string();
	}
}