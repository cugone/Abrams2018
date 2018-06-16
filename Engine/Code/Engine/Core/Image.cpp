#include "Engine/Core/Image.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Win.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "ThirdParty/stb/stb_image.h"
#include "ThirdParty/stb/stb_image_write.h"

#include <filesystem>
#include <sstream>
#include <vector>

Image::Image(const std::string& filePath)
    : m_filepath(filePath)
    , m_memload(false) {

    namespace FS = std::experimental::filesystem;
    FS::path fp(filePath);
    std::vector<unsigned char> buf = {};
    if(FileUtils::ReadBufferFromFile(buf, fp.string())) {
        m_isGif = (buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F' && buf[3] == '8' && (buf[4] == '9' || buf[4] == '7') && buf[5] == 'a');
        if(!m_isGif) {
            m_texelBytes = stbi_load_from_memory(buf.data(), buf.size(), &m_dimensions.x, &m_dimensions.y, &m_bytesPerTexel, 4);
        } else {
            int depth = 0;
            int* delays = nullptr;
            m_texelBytes = stbi_load_gif_from_memory(buf.data(), buf.size(), &delays, &m_dimensions.x, &m_dimensions.y, &depth, &m_bytesPerTexel, 4);
            m_gifDelays.resize(depth);
            for(int i = 0; i < depth; ++i) {
                m_gifDelays[i] = delays[i];
            }
            m_dimensions.y *= depth;
        }
    } else {
        std::ostringstream ss;
        ss << "Failed to load image. " << fp << " is not a supported image type.";
        ASSERT_OR_DIE(m_texelBytes != nullptr, ss.str());
    }
}
Image::Image(unsigned int width, unsigned int height)
    : m_dimensions(width, height)
    , m_bytesPerTexel(4)
    , m_memload(true) {
    m_texelBytes = new unsigned char[width * height * m_bytesPerTexel];
    std::fill(m_texelBytes, m_texelBytes + width * height * m_bytesPerTexel, static_cast<unsigned char>(0));
}

Image::Image(unsigned char* data, unsigned int width, unsigned int height)
    : m_dimensions(width, height)
    , m_bytesPerTexel(4)
    , m_memload(true) {
    m_texelBytes = new unsigned char[width * height * m_bytesPerTexel];
    std::memcpy(m_texelBytes, data, width * height * m_bytesPerTexel);
}

Image::Image(Rgba* data, unsigned int width, unsigned int height)
    : m_dimensions(width, height)
    , m_bytesPerTexel(4)
    , m_memload(true) {
    m_texelBytes = new unsigned char[width * height * m_bytesPerTexel];
    std::memcpy(m_texelBytes, data, width * height * m_bytesPerTexel);
}

Image::Image(const std::vector<Rgba>& data, unsigned int width, unsigned int height)
    : m_dimensions(width, height)
    , m_bytesPerTexel(4)
    , m_memload(true) {
    m_texelBytes = new unsigned char[width * height * m_bytesPerTexel];
    std::memcpy(m_texelBytes, data.data(), width * height * m_bytesPerTexel);
}

Image::Image(const std::vector<unsigned char>& data, unsigned int width, unsigned int height)
    : m_dimensions(width, height)
    , m_bytesPerTexel(4)
    , m_memload(true) {
    m_texelBytes = new unsigned char[width * height * m_bytesPerTexel];
    std::memcpy(m_texelBytes, data.data(), width * height * m_bytesPerTexel);
}

Image::Image(Image&& img)
: m_dimensions(std::move(img.m_dimensions))
, m_bytesPerTexel(std::move(img.m_bytesPerTexel))
, m_filepath(std::move(img.m_filepath))
, m_memload(std::move(img.m_memload))
, m_texelBytes(std::move(img.m_texelBytes)) {

    img.m_texelBytes = nullptr;
    m_dimensions = IntVector2::ZERO;
    m_bytesPerTexel = 0;
    m_filepath = "";
    m_memload = false;
}

Image& Image::operator=(Image&& rhs) {
    if(this == &rhs) {
        return *this;
    }
    if(m_memload) {
        delete[] m_texelBytes;
        m_texelBytes = nullptr;
    } else {
        stbi_image_free(m_texelBytes);
        m_texelBytes = nullptr;
    }

    m_texelBytes = rhs.m_texelBytes;
    m_dimensions = rhs.m_dimensions;
    m_filepath = rhs.m_filepath;
    m_bytesPerTexel = rhs.m_bytesPerTexel;
    m_memload = m_memload;

    rhs.m_texelBytes = nullptr;
    rhs.m_dimensions = IntVector2::ZERO;
    rhs.m_filepath = "";
    rhs.m_bytesPerTexel = 0;
    rhs.m_memload = false;

    return *this;
}
Image::~Image() {
    if(m_memload) {
        delete[] m_texelBytes;
        m_texelBytes = nullptr;
    } else {
        stbi_image_free(m_texelBytes);
        m_texelBytes = nullptr;
    }
}
Rgba Image::GetTexel(const IntVector2& texelPos) const {

    int index = texelPos.x + texelPos.y * m_dimensions.x;
    int byteOffset = index * m_bytesPerTexel;
    //HACK: If too slow, use following commented line instead.
    Rgba color;// = *(reinterpret_cast<Rgba>(&m_texelBytes[byteOffset]));
    color.r = m_texelBytes[byteOffset + 0];
    color.g = m_texelBytes[byteOffset + 1];
    color.b = m_texelBytes[byteOffset + 2];
    if(m_bytesPerTexel == 4) {
        color.a = m_texelBytes[byteOffset + 3];
    } else {
        color.a = 255;
    }
    return color;
}
void Image::SetTexel(const IntVector2& texelPos, const Rgba& color) {
    Rgba oldColor = GetTexel(texelPos);
    int index = texelPos.x + texelPos.y * m_dimensions.x;
    int byteOffset = index * m_bytesPerTexel;
    //HACK: If too slow, use following commented line instead.
    //m_texelBytes[byteOffset] = reinterpret_cast<unsigned char*>(&color);
    m_texelBytes[byteOffset + 0] = color.r;
    m_texelBytes[byteOffset + 1] = color.g;
    m_texelBytes[byteOffset + 2] = color.b;
    if(m_bytesPerTexel == 4) {
        m_texelBytes[byteOffset + 3] = color.a;
    }
}

const std::string& Image::GetFilepath() const {
    return m_filepath;
}

const IntVector2& Image::GetDimensions() const {
    return m_dimensions;
}

unsigned char* Image::GetData() const {
    return m_texelBytes;
}

const std::vector<int>& Image::GetDelaysIfGif() const {
    return m_gifDelays;
}

bool Image::Export(const std::string& filepath, int bytes_per_pixel /*= 4*/, int jpg_quality /*= 100*/) {

    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    std::string extension = StringUtils::ToLowerCase(p.extension().string());

    const auto& dims = this->GetDimensions();
    int w = dims.x;
    int h = dims.y;
    int bbp = bytes_per_pixel;
    int stride = bbp * w;
    int quality = MathUtils::Clamp(jpg_quality, 0, 100);
    int result = 0;
    if(extension == ".png") {
        std::scoped_lock<std::mutex> lock(_cs);
        result = stbi_write_png(filepath.c_str(), w, h, bbp, m_texelBytes, stride);
    } else if(extension == ".bmp") {
        std::scoped_lock<std::mutex> lock(_cs);
        result = stbi_write_bmp(filepath.c_str(), w, h, bbp, m_texelBytes);
    } else if(extension == ".tga") {
        std::scoped_lock<std::mutex> lock(_cs);
        result = stbi_write_tga(filepath.c_str(), w, h, bbp, m_texelBytes);
    } else if(extension == ".jpg") {
        std::scoped_lock<std::mutex> lock(_cs);
        result = stbi_write_jpg(filepath.c_str(), w, h, bbp, m_texelBytes, quality);
    } else if(extension == ".hdr") {
        std::vector<float> data;
        data.reserve(w * h * bbp);
        for(std::size_t i = 0; i < static_cast<std::size_t>(w * h); i += bbp) {
            float color[4];
            color[0] = m_texelBytes[i] / 255.0f;
            if(bbp > 1) { //YA
                color[1] = m_texelBytes[i + 1] / 255.0f;
                if(bbp > 2) { //RGB
                    color[2] = m_texelBytes[i + 2] / 255.0f;
                    if(bbp > 3) { //RGBA
                        color[3] = m_texelBytes[i + 3] / 255.0f;
                    } else { //RGB
                        color[3] = 1.0f;
                    }
                } else { //YA
                    color[3] = color[1];
                    color[1] = color[2] = color[0];
                }
            } else { //Y
                color[3] = 1.0f;
                color[1] = color[2] = color[0];
            }
            data.push_back(color[0]);
            data.push_back(color[1]);
            data.push_back(color[2]);
            data.push_back(color[3]);
        }
        {
            std::scoped_lock<std::mutex> lock(_cs);
            result = stbi_write_hdr(filepath.c_str(), w, h, bbp, data.data());
        }
    }
    return 0 != result;
}

Image* Image::CreateImageFromFileBuffer(const std::vector<unsigned char>& data) {
    if(data.empty()) {
        return nullptr;
    }
    Image* result = new Image;
    int comp = 0;
    int req_comp = 4;
    result->m_texelBytes = stbi_load_from_memory(data.data(), data.size(), &result->m_dimensions.x, &result->m_dimensions.y, &comp, req_comp);
    result->m_bytesPerTexel = comp;
    result->m_memload = false;
    return result;
}
