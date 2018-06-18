#include "Engine/Audio/Wav.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include <sstream>

namespace FileUtils {

namespace WavChunkID {
constexpr const bool IsValid(const char* id) {
    return (StringUtils::FourCC(id) == WavChunkID::FMT
            || StringUtils::FourCC(id) == WavChunkID::FACT
            || StringUtils::FourCC(id) == WavChunkID::DATA);
}
} //End WavChunkID

Wav::~Wav() {
    delete _riff_data;
    _riff_data = nullptr;
}

unsigned int Wav::Load(const std::string& filepath) {
    _riff_data = new Riff;
    if(_riff_data->Load(filepath) == Riff::RIFF_SUCCESS) {
        std::stringstream ss(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
        auto next_chunk = _riff_data->GetNextChunk();
        if(next_chunk->data != nullptr) {
            bool is_wave = StringUtils::FourCC(next_chunk->data->fourcc) == RiffChunkID::WAVE;
            if(!is_wave) {
                delete _riff_data;
                _riff_data = nullptr;
                return WAV_ERROR_NOT_A_WAV;
            }
            ss.write(reinterpret_cast<const char*>(next_chunk->data->data), next_chunk->header.length - 4);
            ss.clear();
            ss.seekp(0);
            ss.seekg(0);
            WavHeader cur_header{};
            while(ss.read(reinterpret_cast<char*>(&cur_header), sizeof(cur_header))) {
                switch(StringUtils::FourCC(cur_header.fourcc)) {
                    case WavChunkID::FMT:
                    {
                        if(!ss.read(reinterpret_cast<char*>(&_fmt), cur_header.length)) {
                            delete _riff_data;
                            _riff_data = nullptr;
                            return WAV_ERROR_BAD_FILE;
                        }
                        break;
                    }
                    case WavChunkID::DATA:
                    {
                        _data.length = cur_header.length;
                        _data.data = new uint8_t[_data.length];
                        if(!ss.read(reinterpret_cast<char*>(_data.data), _data.length)) {
                            delete[] _data.data;
                            _data.data = nullptr;
                            delete _riff_data;
                            _riff_data = nullptr;
                            return WAV_ERROR_BAD_FILE;
                        }
                        break;
                    }
                    case WavChunkID::FACT:
                    {
                        if(!ss.read(reinterpret_cast<char*>(&_fact), cur_header.length)) {
                            delete[] _data.data;
                            _data.data = nullptr;
                            delete _riff_data;
                            _riff_data = nullptr;
                            return WAV_ERROR_BAD_FILE;
                        }
                        break;
                    }
                    default:
                    {
                        ss.seekp(cur_header.length, std::ios_base::cur);
                        ss.seekg(cur_header.length, std::ios_base::cur);
                    }
                }
            }
        }
        return WAV_SUCCESS;
    }
    return WAV_ERROR_NOT_A_WAV;
}

unsigned char* Wav::GetFormatAsBuffer() {
    return reinterpret_cast<unsigned char*>(&_fmt);
}

unsigned char* Wav::GetDataBuffer() const {
    return _data.data;
}

uint32_t Wav::GetDataBufferSize() const {
    return _data.length;
}

const Wav::WavFormatChunk& Wav::GetFormatChunk() const {
    return _fmt;
}

const Wav::WavDataChunk& Wav::GetDataChunk() const {
    return _data;
}

} //End FileUtils