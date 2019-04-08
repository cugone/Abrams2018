#include "Engine/Video/Avi.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include <sstream>

namespace FileUtils {

    namespace AviChunkID {
        constexpr const bool IsValid(const char* id) {
            return (StringUtils::FourCC(id) == AviChunkID::HDRL
                || StringUtils::FourCC(id) == AviChunkID::MOVI
                || StringUtils::FourCC(id) == AviChunkID::LIST
                || StringUtils::FourCC(id) == AviChunkID::INFO
                || StringUtils::FourCC(id) == AviChunkID::AVIH
                || StringUtils::FourCC(id) == AviChunkID::JUNK);
        }
    } //End AviChunkID

    unsigned int Avi::Load(const std::string& filepath) {
        Riff riff_data{};
        if(riff_data.Load(filepath) != Riff::RIFF_SUCCESS) {
            return AVI_ERROR_NOT_A_AVI;
        }

        if(auto next_chunk = riff_data.GetNextChunk()) {
            if(!next_chunk->data) {
                return AVI_SUCCESS; //Successfully read an empty file!
            }
            bool is_avi = StringUtils::FourCC(next_chunk->data->fourcc) == RiffChunkID::AVI;
            if(!is_avi) {
                return AVI_ERROR_NOT_A_AVI;
            }

            std::stringstream ss(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
            ss.write(reinterpret_cast<const char*>(next_chunk->data->subdata.get()), next_chunk->header.length - 4);
            ss.clear();
            ss.seekp(0);
            ss.seekg(0);
            AviHeader riff_header{};
            if(!ss.read(reinterpret_cast<char*>(&riff_header), sizeof(riff_header))) {
                return AVI_ERROR_NOT_A_AVI;
            }
            if(StringUtils::FourCC(riff_header.fourcc) == RiffChunkID::LIST) {
                auto chunk = std::make_unique<AviSubChunk>();
                chunk->data_length = riff_header.length - 4;
                if(!ss.read(reinterpret_cast<char*>(chunk->fourcc), 4)) {
                    return AVI_ERROR_BAD_FILE;
                }
                chunk->subdata = std::move(std::make_unique<uint8_t[]>(chunk->data_length));
                if(!ss.read(reinterpret_cast<char*>(chunk->subdata.get()), chunk->data_length)) {
                    return AVI_ERROR_BAD_FILE;
                }
                switch(StringUtils::FourCC(chunk->fourcc)) {
                case AviChunkID::HDRL:
                {
                    std::stringstream ss_hdrl(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
                    ss_hdrl.write(reinterpret_cast<char*>(chunk->subdata.get()), chunk->data_length);
                    ss_hdrl.clear();
                    ss_hdrl.seekp(0);
                    ss_hdrl.seekg(0);
                    AviHeader avih{};
                    if(!ss_hdrl.read(reinterpret_cast<char*>(&avih), sizeof(avih))) {
                        return AVI_ERROR_BAD_FILE;
                    }
                    auto avih_chunk = std::make_unique<AviSubChunk>();
                    avih_chunk->data_length = avih.length - 4;
                    avih_chunk->fourcc[0] = avih.fourcc[0];
                    avih_chunk->fourcc[1] = avih.fourcc[1];
                    avih_chunk->fourcc[2] = avih.fourcc[2];
                    avih_chunk->fourcc[3] = avih.fourcc[3];
                    avih_chunk->subdata = std::move(std::make_unique<uint8_t[]>(avih_chunk->data_length));
                    if(!ss_hdrl.read(reinterpret_cast<char*>(avih_chunk->subdata.get()), avih_chunk->data_length)) {
                        return AVI_ERROR_BAD_FILE;
                    }
                    if(StringUtils::FourCC(avih_chunk->fourcc) == AviChunkID::AVIH) {
                        std::stringstream ss_avih(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
                        ss_avih.write(reinterpret_cast<char*>(avih_chunk->subdata.get()), avih_chunk->data_length);
                        ss_avih.clear();
                        ss_avih.seekp(0);
                        ss_avih.seekg(0);
                        if(!ss_avih.read(reinterpret_cast<char*>(&_hdrl), avih_chunk->data_length)) {
                            return AVI_ERROR_BAD_FILE;
                        }
                        _frames.reserve(GetFrameCount());
                    }
                    break;
                }
                case AviChunkID::MOVI:
                {
                    AviMoviChunk frame{};
                    frame.length = riff_header.length;
                    frame.data = std::move(std::make_unique<uint8_t[]>(frame.length));
                    if(!ss.read(reinterpret_cast<char*>(frame.data.get()), frame.length)) {
                        return AVI_ERROR_BAD_FILE;
                    }
                    _frames.emplace_back(frame.length, std::move(frame.data));
                    break;
                }
                case AviChunkID::JUNK:
                {
                    {
                        std::ostringstream err_ss{};
                        err_ss << "JUNK AVI Chunk.";
                        err_ss << " Length: " << riff_header.length << '\n';
                        DebuggerPrintf(err_ss.str().c_str());
                    }
                    ss.seekp(riff_header.length, std::ios_base::cur);
                    ss.seekg(riff_header.length, std::ios_base::cur);
                    break;
                }
                case AviChunkID::INFO:
                {
                    {
                        std::ostringstream err_ss{};
                        err_ss << "INFO AVI Chunk.";
                        err_ss << " Length: " << riff_header.length << '\n';
                        DebuggerPrintf(err_ss.str().c_str());
                    }
                    ss.seekp(riff_header.length, std::ios_base::cur);
                    ss.seekg(riff_header.length, std::ios_base::cur);
                    break;
                }
                default:
                {
                    {
                        std::ostringstream err_ss{};
                        err_ss << "Unknown AVI Chunk ID: ";
                        err_ss << chunk->fourcc[0]
                            << chunk->fourcc[1]
                            << chunk->fourcc[2]
                            << chunk->fourcc[3];
                        err_ss << " Length: " << riff_header.length << '\n';
                        DebuggerPrintf(err_ss.str().c_str(), "%s");
                    }
                    ss.seekp(riff_header.length, std::ios_base::cur);
                    ss.seekg(riff_header.length, std::ios_base::cur);
                    break;
                }
                }
            }
        }
        return AVI_SUCCESS;
    }

    const FileUtils::Avi::AviHdrlChunk& Avi::GetHdrlChunk() const {
        return _hdrl;
    }

    const FileUtils::Avi::AviMoviChunk* Avi::GetFrame(std::size_t frame_idx) const {
        if(frame_idx >= _frames.size()) {
            return nullptr;
        }
        return &_frames[frame_idx];
    }
    
    const std::size_t Avi::GetFrameCount() const {
        return _hdrl.totalFrames;
    }

    TimeUtils::FPSeconds Avi::GetLengthInSeconds() const {
        return TimeUtils::FPSeconds{GetLengthInMicroSeconds()};
    }

    TimeUtils::FPMicroseconds Avi::GetLengthInMicroSeconds() const {
        return TimeUtils::FPMicroseconds{_hdrl.usPerFrame * _hdrl.totalFrames};
    }

    IntVector2 Avi::GetFrameDimensions() const {
        return IntVector2{ static_cast<int>(_hdrl.width), static_cast<int>(_hdrl.height) };
    }

} //End FileUtils