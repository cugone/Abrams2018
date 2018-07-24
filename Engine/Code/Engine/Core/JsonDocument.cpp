#include "Engine/Core/JsonDocument.hpp"

#include "Engine/Core/FileUtils.hpp"

#include <filesystem>
#include <fstream>

#include <string>

namespace DataUtils::Json {

    void Document::Parse(const std::string& /*json_string*/) {

    }

    void Document::LoadFile(const std::string& json_filepath) {
        std::filesystem::path p(json_filepath);
        p.make_preferred();
        std::ifstream json_ifstream;
        std::string file_contents;
        json_ifstream.open(p.string());

        std::string cur_line;
        while(std::getline(json_ifstream, cur_line)) {
            file_contents += cur_line;
            file_contents += "\n";
        }
        json_ifstream.close();
    }

    void Document::SetState(const ParseState& newState) {
        _state = newState;
    }

    Object Document::ParseObject(const std::string::const_iterator& /*iter*/) {

    }

    Array Document::ParseArray(const std::string::const_iterator& /*iter*/) {

    }

    Value Document::ParseValue(const std::string::const_iterator& /*iter*/) {

    }

    String Document::ParseString(const std::string::const_iterator& /*iter*/) {

    }

    Number Document::ParseNumber(const std::string::const_iterator& /*iter*/) {

    }

    void Document::AdvanceCur(int amount /*= 1*/) {
        std::advance(_cur_char, amount);
    }

    void Document::AdvanceNext(int amount /*= 1*/) {
        std::advance(_next_char, amount);
    }

    void Document::AdvancePrev(int amount /*= 1*/) {
        std::advance(_prev_char, amount);
    }

    void Document::SyncPrev() {
        _prev_char = _cur_char;
    }

    void Document::SyncCur() {
        _cur_char = _next_char;
    }

    void Document::SyncPrevAndCur() {
        SyncPrev();
        SyncCur();
    }

    void Document::SyncCurAndPrev() {
        SyncPrevAndCur();
    }

}