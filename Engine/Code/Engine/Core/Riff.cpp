#include "Engine/Core/Riff.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

#include <iostream>
#include <sstream>

namespace FileUtils {

namespace RiffChunkID {
constexpr const bool IsValid(const char* id) {
    return (StringUtils::FourCC(id) == RiffChunkID::RIFF
            || StringUtils::FourCC(id) == RiffChunkID::LIST
            || StringUtils::FourCC(id) == RiffChunkID::INFO
            || StringUtils::FourCC(id) == RiffChunkID::WAVE);
}
} //End RiffChunkID

bool Riff::ParseDataIntoChunks(std::vector<unsigned char>& buffer) {
    std::stringstream stream(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    stream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    buffer.clear();
    buffer.shrink_to_fit();
    stream.clear();
    stream.seekp(0);
    stream.seekg(0);
    bool is_first_chunk = true;
    RiffHeader cur_header{};
    while(stream.read(reinterpret_cast<char*>(&cur_header), sizeof(cur_header))) {
        auto cur_chunk = std::make_unique<RiffChunk>();
        if(is_first_chunk && StringUtils::FourCC(cur_header.fourcc) != RiffChunkID::RIFF) {
            return false;
        }
        is_first_chunk = false;
        cur_chunk->header = cur_header;
        switch(StringUtils::FourCC(cur_header.fourcc)) {
            case RiffChunkID::RIFF:
            {
                auto subdata = std::make_unique<RiffSubChunk>();
                if(!stream.read(reinterpret_cast<char*>(&subdata->fourcc), 4)) {
                    return false;
                }
                subdata->data = std::move(std::make_unique<uint8_t[]>(cur_header.length - 4));
                if(!stream.read(reinterpret_cast<char*>(subdata->data.get()), cur_header.length - 4)) {
                    return false;
                }
                cur_chunk->data = std::move(subdata);
                break;
            }
            case RiffChunkID::INFO:
            {
                break;
            }
            case RiffChunkID::LIST:
            {
                auto subdata = std::make_unique<RiffSubChunk>();
                if(!stream.read(reinterpret_cast<char*>(&subdata->fourcc), 4)) {
                    return false;
                }
                subdata->data = std::move(std::make_unique<uint8_t[]>(cur_header.length - 4));
                if(!stream.read(reinterpret_cast<char*>(subdata->data.get()), cur_header.length - 4)) {
                    return false;
                }
                cur_chunk->data = std::move(subdata);
                break;
            }
            default:
            {
                stream.seekp(cur_header.length, std::ios_base::cur);
                stream.seekg(cur_header.length, std::ios_base::cur);
                break;
            }
        }
        _chunks.push_back(std::move(cur_chunk));
    }
    _current_chunk = _chunks.begin();
    return true;
}

void Riff::ShowRiffChunkHeaders() {
#ifdef AUDIO_DEBUG
    std::ostringstream ss;
    ss << "Chunks:\n";
    for(auto& chunk : _chunks) {
        ss << "Chunk ID: ";
        ss << chunk->header.fourcc[0]
            << chunk->header.fourcc[1]
            << chunk->header.fourcc[2]
            << chunk->header.fourcc[3]
            << '\n';
        ss << "Length: " << chunk->header.length << '\n';
        ss << "------------\n";
    }
    ss.flush();
    DebuggerPrintf(ss.str().c_str());
#endif
}

Riff::RiffChunk* Riff::GetNextChunk() {
    if(_current_chunk == _chunks.end()) {
        return nullptr;
    }
    auto chunk = (*_current_chunk).get();
    _current_chunk++;
    return chunk;
}

unsigned int Riff::Load(const std::string& filename) {
    std::vector<unsigned char> buffer{};
    if(!FileUtils::ReadBufferFromFile(buffer, filename)) {
        return RIFF_ERROR_INVALID_ARGUMENT;
    }
    if(!ParseDataIntoChunks(buffer)) {
        return RIFF_ERROR_NOT_A_RIFF;
    }
    ShowRiffChunkHeaders();
    return RIFF_SUCCESS;
}

unsigned int Riff::Load(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> buffer = data;
    if(!ParseDataIntoChunks(buffer)) {
        return RIFF_ERROR_NOT_A_RIFF;
    }
    ShowRiffChunkHeaders();
    return RIFF_SUCCESS;
}

} //End FileUtils