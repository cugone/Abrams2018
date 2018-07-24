#pragma once

#include <string>

#include "Engine/Core/Json.hpp"

namespace DataUtils::Json {

    class Document {
    private:
        enum class ParseState {
            None,
            Object,
            Array,
            Value,
            String,
            Number,
        };
    public:
        Document() = default;
        ~Document() = default;

        void Parse(const std::string& json_string);
        void LoadFile(const std::string& json_filepath);
    protected:
        void SetState(const ParseState& newState);
        Object ParseObject(const std::string::const_iterator& iter);
        Array ParseArray(const std::string::const_iterator& iter);
        Value ParseValue(const std::string::const_iterator& iter);
        String ParseString(const std::string::const_iterator& iter);
        Number ParseNumber(const std::string::const_iterator& iter);
    private:
        void AdvanceCur(int amount = 1);
        void AdvanceNext(int amount = 1);
        void AdvancePrev(int amount = 1);
        void SyncPrev();
        void SyncCur();
        void SyncPrevAndCur();
        void SyncCurAndPrev();
        ParseState _state = ParseState::None;
        std::vector<unsigned char> _file_contents = {};
        decltype(_file_contents)::const_iterator _prev_char = {};
        decltype(_file_contents)::const_iterator _cur_char = {};
        decltype(_file_contents)::const_iterator _next_char = {};

    };

} //End DataUtils