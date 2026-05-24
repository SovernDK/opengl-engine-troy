#pragma once
#include "SDL3/SDL.h"

#include <string>
#include <glad/glad.h>

#include "lib/stb_image.h"

struct WrapAxis
{
    bool x = true;
    bool y = true;
};

class Texture2D
{
public:
    ~Texture2D()
    {
        if (id != 0)
            glDeleteTextures(1, &id);
    }

    //Expand texture creation
    void create(unsigned int width, unsigned int height, unsigned char* data)
    {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, data);

        if (wrapAxis.x) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
        if (wrapAxis.y) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);

        if (wrapping == GL_CLAMP_TO_BORDER)
        {
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

        if (mipMap)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipMap);
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        this->width = width;
        this->height = height;
    }

    bool load(const std::string& path)
    {
        //stbi_set_flip_vertically_on_load(true);

        int channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if (!data)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "Failed to load texture: %s (reason: %s)",
                path.c_str(),
                stbi_failure_reason());

            return false;
        }

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        if (wrapAxis.x) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
        if (wrapAxis.y) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);

        if (wrapping == GL_CLAMP_TO_BORDER)
        {
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipMap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }

    void bind(int slot = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void setChannels(GLenum channels)
    {
        this->channels = channels;
    }

    void setFiltering(GLenum filtering)
    {
        this->filtering = filtering;
    }

    void setWrapping(GLenum wrapping)
    {
        this->wrapping = wrapping;
    }

    void setMipMap(GLenum mipmap)
    {
        this->mipMap = mipmap;
		mipMaps = true;
    }

    void setWrapAxis(bool x, bool y)
    {
        wrapAxis.x = x;
        wrapAxis.y = y;
    }

    void enableMipMap(bool enable)
    {
        mipMaps = enable;
	}

    void setBorderColor(float r, float g, float b)
    {
        borderColor[0] = r;
        borderColor[1] = g;
        borderColor[2] = b;
    }

    int getWidth() const
    {
        return width;
    }

    int getHeight() const
    {
        return height;
    }

    GLuint ID() const
    {
        return id;
    }
private:
    GLuint id = 0;
    int width = 0;
    int height = 0;
    GLenum channels = GL_RGB;
    GLenum filtering = GL_NEAREST;
    GLenum wrapping = GL_REPEAT;
    
    bool mipMaps = false;
    GLenum mipMap = GL_NEAREST_MIPMAP_LINEAR;

    WrapAxis wrapAxis;
    float borderColor[3] = { 1.0, 1.0, 1.0 };
};