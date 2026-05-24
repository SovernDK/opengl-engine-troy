#pragma once
#include "SDL3/SDL.h"

#include "glm/glm.hpp"

#include <ext/scalar_constants.hpp>
#include <random>
#include <optional>

namespace vector {
    inline bool equal(glm::vec2 a, glm::vec2 b)
    {
        return glm::distance(a, b) < glm::epsilon<float>();
    }
}

namespace random
{
    static std::mt19937 gen;

    inline void Init(std::optional<uint32_t> seed)
    {
        if (seed.has_value())
            gen.seed(seed.value());
        else
            gen.seed(std::random_device{}());
    }

    inline uint32_t Seed()
    {
        return std::uniform_int_distribution<uint32_t>(0, std::numeric_limits<uint32_t>::max())(gen);
    }

    inline int Int(int min, int max)
    {
        if (min > max) {
            std::swap(min, max);
            SDL_Log("Warning: min value was greater than max value in Random::Int, swapping values to avoid issues!");
        }
        else if (max == min)
        {
            SDL_Log("Warning: min value was equal to max value in Random::Int, returning the only possible value!");
            return min;
        }

        return std::uniform_int_distribution<>(min, max)(gen);
    }

    inline float Float(float min, float max)
    {
        if (min > max) {
            std::swap(min, max);
            SDL_Log("Warning: min value was greater than max value in Random::Float, swapping values to avoid issues!");
        }
        else if (max == min)
        {
            SDL_Log("Warning: min value was equal to max value in Random::Float, returning the only possible value!");
            return min;
        }

        return std::uniform_real_distribution<>(min, max)(gen);
    }

    inline bool Chance(float probability)
    {
        return std::uniform_real_distribution<>(0.0f, 1.0f)(gen) < probability;
    }
}

namespace text
{
    inline std::vector<std::string> split(const std::string& text, std::string delimeter)
    {
        std::vector<std::string> result;
        size_t start = 0;

        for (size_t found = text.find(delimeter); found != std::string::npos; found = text.find(delimeter, start))
        {
            result.emplace_back(text.begin() + start, text.begin() + found);
            start = found + delimeter.size();
        }
        if (start != text.size())
            result.emplace_back(text.begin() + start, text.end());
        return result;
    }
}

namespace color
{
    inline SDL_Color lerp(const SDL_Color& a, const SDL_Color& b, float t)
    {
        return {
            static_cast<uint8_t>(a.r + t * (b.r - a.r)),
            static_cast<uint8_t>(a.g + t * (b.g - a.g)),
            static_cast<uint8_t>(a.b + t * (b.b - a.b)),
            static_cast<uint8_t>(a.a + t * (b.a - a.a))
        };
    }
}

namespace utils
{
    inline float normalize(float value, float min, float max)
    {
        if (min > max) {
            std::swap(min, max);
            SDL_Log("Warning: min value was greater than max value in utils::normalize, swapping values to avoid issues!");
        }
        else if (max == min)
        {
            SDL_Log("Warning: min value was equal to max value in utils::normalize, returning 0.0f to avoid division by zero!");
            return 0.0f;
        }
        return (value - min) / (max - min);
    }
}