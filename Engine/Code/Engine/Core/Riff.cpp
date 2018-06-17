#include "Engine/Core/Riff.hpp"

#include "Engine/Core/BuildConfig.cpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

#include <iostream>
#include <sstream>

Riff::~Riff() {
    _chunks.clear();
    _chunks.shrink_to_fit();
}

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
        RiffChunk* cur_chunk = new RiffChunk;
        if(is_first_chunk && StringUtils::FourCC(cur_header.fourcc) != Riff::ID::RIFF) {
            delete cur_chunk;
            cur_chunk = nullptr;
            return false;
        }
        is_first_chunk = false;
        cur_chunk->header = cur_header;
        switch(StringUtils::FourCC(cur_header.fourcc)) {
            case Riff::ID::RIFF:
            {
                RiffSubChunk* subdata = new RiffSubChunk;
                if(!stream.read(reinterpret_cast<char*>(&subdata->fourcc), 4)) {
                    delete subdata;
                    subdata = nullptr;
                    delete cur_chunk;
                    cur_chunk = nullptr;
                    return false;
                }
                subdata->data = new uint8_t[cur_header.length - 4];
                if(!stream.read(reinterpret_cast<char*>(subdata->data), cur_header.length - 4)) {
                    delete subdata;
                    subdata = nullptr;
                    delete cur_chunk;
                    cur_chunk = nullptr;
                    return false;
                }
                cur_chunk->data = subdata;
                break;
            }
            case Riff::ID::INFO:
            {
                break;
            }
            case Riff::ID::LIST:
            {
                RiffSubChunk* subdata = new RiffSubChunk;
                if(!stream.read(reinterpret_cast<char*>(&subdata->fourcc), 4)) {
                    delete subdata;
                    subdata = nullptr;
                    delete cur_chunk;
                    cur_chunk = nullptr;
                    return false;
                }
                subdata->data = new uint8_t[cur_header.length - 4];
                if(!stream.read(reinterpret_cast<char*>(subdata->data), cur_header.length - 4)) {
                    delete subdata;
                    subdata = nullptr;
                    delete cur_chunk;
                    cur_chunk = nullptr;
                    return false;
                }
                cur_chunk->data = subdata;
                break;
            }
            default:
            {
                stream.seekp(cur_header.length, std::ios_base::cur);
                stream.seekg(cur_header.length, std::ios_base::cur);
                break;
            }
        }
        _chunks.push_back(cur_chunk);
    }
    _current_chunk = _chunks.begin();
    return true;
}

bool Riff::IsRecognizableChunk(const char* id) const {
    return (StringUtils::FourCC(id) == Riff::ID::RIFF
            || StringUtils::FourCC(id) == Riff::ID::LIST
            || StringUtils::FourCC(id) == Riff::ID::INFO);
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

Riff::RiffChunk* Riff::GetNextChunk() const {
    if(_current_chunk == _chunks.end()) {
        return nullptr;
    }
    auto chunk = *_current_chunk;
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
