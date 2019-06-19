#pragma once

#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/IntVector2.hpp"

#include <mutex>
#include <string>
#include <filesystem>

class Image {
public:
    explicit Image(std::filesystem::path filepath) noexcept;
    Image(const Image& img) = delete;
    Image(Image&& img) noexcept;
    Image& operator=(const Image& rhs) = delete;
    Image& operator=(Image&& rhs) noexcept;

    Image(unsigned char* data, unsigned int width, unsigned int height) noexcept;
    Image(Rgba* data, unsigned int width, unsigned int height) noexcept;
    Image(unsigned int width, unsigned int height) noexcept;
    Image(const std::vector<Rgba>& data, unsigned int width, unsigned int height) noexcept;
    Image(const std::vector<unsigned char>& data, unsigned int width, unsigned int height) noexcept;
    ~Image() noexcept;

    Rgba GetTexel(const IntVector2& texelPos) const noexcept;
    void SetTexel(const IntVector2& texelPos, const Rgba& color) noexcept;

    const std::filesystem::path& GetFilepath() const noexcept;
    const IntVector2& GetDimensions() const noexcept;

    unsigned char* GetData() const noexcept;
    std::size_t GetDataLength() const noexcept;
    int GetBytesPerTexel() const noexcept;

    const std::vector<int>& GetDelaysIfGif() const noexcept;
    bool Export(std::filesystem::path filepath, int bytes_per_pixel = 4, int jpg_quality = 100) noexcept;
    static Image* CreateImageFromFileBuffer(const std::vector<unsigned char>& data) noexcept;
    static std::string GetSupportedExtensionsList() noexcept;
    //TODO: Add swap noexcept;
protected:
private:
    Image() = default;
    unsigned char* m_texelBytes = nullptr;
    IntVector2 m_dimensions{};
    unsigned int m_bytesPerTexel = 0;
    std::vector<int> m_gifDelays{};
    std::filesystem::path m_filepath{};
    bool m_memload = false;
    bool m_isGif = false;
    std::mutex _cs{};
};