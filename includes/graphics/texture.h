#pragma once
#include <string>
#include <glad/glad.h>

struct TexID
{
    GLuint id = 1;

    TexID() = default;
    TexID(GLuint _id) { id = _id; }
};

class Texture2D
{
public:
    TexID id   = 0;
	int width   = 0;
	int height  = 0;
public:
    Texture2D() = default;
    ~Texture2D()
    {
        if (id.id != 0)
            glDeleteTextures(1, &id.id);
    }

    Texture2D(Texture2D&& other) noexcept : id(other.id), width(other.width), height(other.height)
    {
        other.id = 0;
        other.width = 0;
        other.height = 0;
	}

    Texture2D& operator=(Texture2D&& other) noexcept
    {
        if (this != &other)
        {
            if (id.id != 0)
                glDeleteTextures(1, &id.id);

            id = other.id;
            //id = other.id;
            width = other.width;
            height = other.height;

            other.id = 0;
            other.width = 0;
            other.height = 0;
        }
        return *this;
	}

    Texture2D(const Texture2D&) = delete;
	Texture2D& operator=(const Texture2D&) = delete;

	int getWidth()  const { return width; }
	int getHeight() const { return height; }
	GLuint ID()     const { return id.id; }
};

class TextureBuilder
{
private:
    GLenum channels       = GL_RGBA;
    GLenum filtering      = GL_NEAREST;
    GLenum wrapping       = GL_REPEAT;
    GLenum mipMap         = GL_NEAREST_MIPMAP_LINEAR;

    GLenum format           = GL_RGBA;
    GLenum internalFormat   = GL_RGBA;

    bool   mipMaps        = true;
    bool   flip           = false;
    bool   wrapAxis[2]    = { true, true };
    float  borderColor[3] = { 1.0f, 1.0f, 1.0f };
public:
    TextureBuilder& setChannels(GLenum channels)
    {
        this->channels = channels;
        return *this;
    }

    TextureBuilder& setFiltering(GLenum filtering)
    {
        this->filtering = filtering;
        return *this;
    }

    TextureBuilder& setFormat(GLenum format)
    {
        this->format = format;
        return *this;
    }

    TextureBuilder& setInternalFormat(GLenum format)
    {
        this->internalFormat = format;
        return *this;
    }

    TextureBuilder& setWrapping(GLenum wrapping)
    {
        this->wrapping = wrapping;
        return *this;
    }

    TextureBuilder& setMipMap(GLenum mipmap)
    {
        this->mipMap = mipmap;
        this->mipMaps = true;
        return *this;
    }

    TextureBuilder& setWrapAxis(bool x, bool y)
    {
        wrapAxis[0] = x;
        wrapAxis[1] = y;
        return *this;
    }

    TextureBuilder& setBorderColor(float r, float g, float b)
    {
        borderColor[0] = r;
        borderColor[1] = g;
        borderColor[2] = b;
        return *this;
    }

    TextureBuilder& flipVertically(bool flip = true)
    {
        this->flip = flip;
        return *this;
    }

    Texture2D build(unsigned int width, unsigned int height, int channels, unsigned char* data)
    {
        Texture2D tex;
        tex.width = width;
        tex.height = height;

        format = (channels == 4) ? GL_RGBA : GL_RGB;
        upload(tex, data);
        return tex;
    }

    // build from raw data
    Texture2D build(unsigned int width, unsigned int height, unsigned char* data)
    {
        Texture2D tex;
        tex.width = width;
        tex.height = height;

        upload(tex, data);
        return tex;
    }
private:
    void upload(Texture2D& tex, unsigned char* data)
    {
        glGenTextures(1, &tex.id.id);
        glBindTexture(GL_TEXTURE_2D, tex.id.id);

        if (wrapAxis[0]) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
        if (wrapAxis[1]) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
        if (wrapping == GL_CLAMP_TO_BORDER)
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipMaps ? mipMap : filtering);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tex.width, tex.height, 0, format, GL_UNSIGNED_BYTE, data);
        if (mipMaps) glGenerateMipmap(GL_TEXTURE_2D);
    }
};