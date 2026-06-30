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
        fs::path logsDir = "";

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

            if (j.contains("logsDir"))
                logsDir = j["logsDir"].get<std::string>();

            SDL_Log("\033[33m---- Config Loaded ----\033[0m");
            SDL_Log("\033[33m defaultFontName:\033[0m %s", defaultFontName.c_str());
            SDL_Log("\033[33m assetsDir:\033[0m %s", assetsDir.string().c_str());
            SDL_Log("\033[33m fontsDir:\033[0m %s", fontsDir.string().c_str());
            SDL_Log("\033[33m shadersDir:\033[0m %s", shadersDir.string().c_str());
            SDL_Log("\033[33m logsDir:\033[0m %s", logsDir.string().c_str());

            SDL_Log("\033[33m Resolved Paths:\033[0m");
            SDL_Log("\033[33m AssetsPath:\033[0m %s", getAssetsPath().string().c_str());
            SDL_Log("\033[33m FontsPath:\033[0m %s", getFontsPath().string().c_str());
            SDL_Log("\033[33m ShadersPath:\033[0m %s", getShadersPath().string().c_str());
            SDL_Log("\033[33m ShadersPath:\033[0m %s", getLogPath().string().c_str());
            SDL_Log("\033[33m DefaultFontPath:\033[0m %s", getDefaultFontPath().string().c_str());
            SDL_Log("\033[33m------------------------\033[0m");

            return true;
        }

        fs::path getAssetsPath() const
        {
            return file_util::createPath(assetsDir.string());
        }

        fs::path getFontsPath() const
        {
            return file_util::createPath(assetsDir.string(), fontsDir.string());
        }

        fs::path getShadersPath() const
        {
            return file_util::createPath(shadersDir.string());
        }

        fs::path getLogPath() const
        {
            return file_util::createPath(logsDir.string());
        }

        fs::path getDefaultFontPath() const
        {
            return file_util::createPath(assetsDir.string(), fontsDir.string(), defaultFontName + ".ttf");
        }
    };

    inline Config GConfig;
}