#pragma once

#include <string>

#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/IntVector2.hpp"

class Image {
public:
    Image(const std::wstring& filePath);
    Image(const Image& img) = delete;
    Image(Image&& img);
    Image& operator=(const Image& rhs) = delete;
    Image& operator=(Image&& rhs);

    Image(unsigned char* data, unsigned int width, unsigned int height);
    Image(Rgba* data, unsigned int width, unsigned int height);
    Image(unsigned int width = 1, unsigned int height = 1);
    Image(const std::vector<Rgba>& data, unsigned int width, unsigned int height);
    Image(const std::vector<unsigned char>& data, unsigned int width, unsigned int height);
    ~Image(); //stbi_image_free(m_texelBytes)

    Rgba GetTexel(const IntVector2& texelPos) const;
    void SetTexel(const IntVector2& texelPos, const Rgba& color);

    const std::wstring& GetFilepath() const;
    const IntVector2& GetDimensions() const;

    unsigned char* GetData() const;

    //bool Export(const std::string& filepath, int bytes_per_pixel = 4, int jpg_quality = 100);
protected:
private:
    unsigned char* m_texelBytes = nullptr;
    IntVector2 m_dimensions{};
    int m_bytesPerTexel = 0;
    std::wstring m_filepath{};
    bool m_memload = false;
    //static CriticalSection _cs;
};