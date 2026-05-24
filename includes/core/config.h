#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "utility/file_util.h"

namespace core
{
    namespace fs = std::filesystem;
    using json = nlohmann::json;

    struct Config
    {
        std::string defaultFontName = "";

        fs::path assetsDir = "";
        fs::path fontsDir = "";
        fs::path shadersDir = "";

        // ---- Loading ----
        bool load(const fs::path& file)
        {
            std::ifstream f(file);
            if (!f.is_open())
                return false;

            json j;
            f >> j;

            if (j.contains("defaultFontName"))
                defaultFontName = j["defaultFontName"].get<std::string>();

            if (j.contains("assetsDir"))
                assetsDir = j["assetsDir"].get<std::string>();

            if (j.contains("fontsDir"))
                fontsDir = j["fontsDir"].get<std::string>();

            if (j.contains("shadersDir"))
                shadersDir = j["shadersDir"].get<std::string>();

            SDL_Log("---- Config Loaded ----");
            SDL_Log("defaultFontName: %s", defaultFontName.c_str());
            SDL_Log("assetsDir: %s", assetsDir.string().c_str());
            SDL_Log("fontsDir: %s", fontsDir.string().c_str());
            SDL_Log("shadersDir: %s", shadersDir.string().c_str());

            SDL_Log("Resolved Paths:");
            SDL_Log("AssetsPath: %s", getAssetsPath().string().c_str());
            SDL_Log("FontsPath: %s", getFontsPath().string().c_str());
            SDL_Log("ShadersPath: %s", getShadersPath().string().c_str());
            SDL_Log("DefaultFontPath: %s", getDefaultFontPath().string().c_str());
            SDL_Log("------------------------");

            return true;
        }

        // ---- Resolved paths (important) ----
        fs::path getAssetsPath() const
        {
            return file_util::getPath(assetsDir.string());
        }

        fs::path getFontsPath() const
        {
            return file_util::getPath(assetsDir.string(), fontsDir.string());
        }

        fs::path getShadersPath() const
        {
            return file_util::getPath(shadersDir.string());
        }

        fs::path getDefaultFontPath() const
        {
            return file_util::getPath(assetsDir.string(), fontsDir.string(), defaultFontName + ".ttf");
        }
    };

    inline Config GConfig;
}