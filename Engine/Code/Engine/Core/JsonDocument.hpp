#pragma once

namespace DataUtils {

    class JsonDocument {
    private:
        enum class ParseState {
            Object,
            Array,
            Value,
            String,
            Number,
        };
    public:
        JsonDocument();
        ~JsonDocument();

        void Parse();

    protected:
    private:
        
    };

} //End DataUtils