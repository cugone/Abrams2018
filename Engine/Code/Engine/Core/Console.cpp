#include "Engine/Core/Console.hpp"

#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "ENgine/Input/InputSystem.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector3.hpp"

#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include <iterator>
#include <sstream>
#include <string>
#include <utility>

Console::Console(Renderer* renderer)
    : _renderer(renderer)
{
    /* DO NOTHING */
}

Console::~Console() {
    delete _camera;
    _camera = nullptr;

    _commands.clear();
    _renderer = nullptr;
}

bool Console::ProcessSystemMessage(const EngineMessage& msg) {

    LPARAM lp = msg.lparam;
    WPARAM wp = msg.wparam;
    switch(msg.wmMessageCode) {
        case WindowsSystemMessage::KEYBOARD_KEYDOWN:
        {
            _non_rendering_char = false;
            unsigned char key = static_cast<unsigned char>(wp);
            uint32_t lpBits = lp;
            //0bTPXRRRRESSSSSSSSCCCCCCCCCCCCCCCC
            //C: repeat count
            //S: scan code
            //E: extended key flag
            //R: reserved
            //X: context code: 0 for KEYDOWN
            //P: previous state 1 for already down
            //T: transition state 0 for KEYDOWN
            constexpr uint32_t repeat_count_mask = 0b0000'0000'0000'0000'1111'1111'1111'1111; //0x0000FFFF;
            constexpr uint32_t scan_code_mask = 0b0000'0000'1111'1111'0000'0000'0000'0000; //0x00FF0000;
            constexpr uint32_t extended_key_mask = 0b0000'0001'0000'0000'0000'0000'0000'0000; //0x01000000;
            constexpr uint32_t reserved_mask = 0b0001'1110'0000'0000'0000'0000'0000'0000; //0x1E000000;
            constexpr uint32_t context_code_mask = 0b0010'0000'0000'0000'0000'0000'0000'0000; //0x20000000;
            constexpr uint32_t previous_state_mask = 0b0100'0000'0000'0000'0000'0000'0000'0000; //0x40000000;
            constexpr uint32_t transition_state_mask = 0b1000'0000'0000'0000'0000'0000'0000'0000; //0x80000000;
            bool is_extended_key = (lpBits & extended_key_mask) != 0;
            if(key < 32 || key == 127) { //Control and DEL chars
                _non_rendering_char = true;
            }
            auto my_key = InputSystem::ConvertWinVKToKeyCode(key);
            if(is_extended_key) {
                if(IsClosed()) {
                    return false;
                }
                switch(my_key) {
                    case KeyCode::ALT:
                    {
                        my_key = KeyCode::RALT;
                        return true;
                    }
                    case KeyCode::CTRL:
                    {
                        my_key = KeyCode::RCTRL;
                        _skip_nonwhitespace_mode = true;
                        break;
                    }
                    case KeyCode::RETURN: PostEntryLine(); return true;
                    case KeyCode::LWIN: return true;
                    case KeyCode::RWIN: return true;
                    case KeyCode::SHIFT:
                    {
                        my_key = KeyCode::RSHIFT;
                        break;
                    }
                    case KeyCode::LEFT: //Left Arrow in cluster on 108-keyboards
                    {
                        auto offset = std::distance(std::cbegin(_entryline), _cursor_position);
                        auto offset_from_previous_space = _entryline.find_last_of(' ', offset);
                        bool has_space = offset_from_previous_space != std::string::npos;
                        MoveCursorLeft(_skip_nonwhitespace_mode && has_space ? offset_from_previous_space : 1);
                        return true;
                    }
                    case KeyCode::RIGHT: //Right Arrow in cluster on 108-keyboards
                    {
                        auto offset = std::distance(std::cend(_entryline) - 1, _cursor_position);
                        auto offset_from_next_space = _entryline.find_first_of(' ', offset);
                        bool has_space = offset_from_next_space != std::string::npos;
                        MoveCursorRight(_skip_nonwhitespace_mode && has_space ? offset_from_next_space : 1);
                        return true;
                    }
                    default: return false;
                }
            }
            switch(my_key) {
                case KeyCode::TILDE:
                {
                    ToggleConsole();
                    if(IsOpen()) {
                        _non_rendering_char = true;
                        _entryline.clear();
                    }
                    return true;
                }
                case KeyCode::RETURN:
                {
                    if(IsClosed()) {
                        return false;
                    }
                    PostEntryLine();
                    return true;
                }
                case KeyCode::BACKSPACE:
                {
                    if(IsClosed()) {
                        return false;
                    }
                    return true;
                }
                case KeyCode::DELETE_KEYCODE:
                {
                    if(IsClosed()) {
                        return false;
                    }
                    return true;
                }
                case KeyCode::ESCAPE:
                {
                    if(IsClosed()) {
                        return false;
                    }
                    ToggleConsole();
                    return true;
                }
                case KeyCode::LEFT:
                {
                    auto offset = std::distance(std::cbegin(_entryline), _cursor_position);
                    MoveCursorLeft(_skip_nonwhitespace_mode ? _entryline.find_last_of(' ', offset) : 1);
                    return true;
                }
                case KeyCode::RIGHT:
                {
                    auto offset = std::distance(std::cend(_entryline) - 1, _cursor_position);
                    MoveCursorRight(_skip_nonwhitespace_mode ? _entryline.find_first_of(' ', offset) : 1);
                    return true;
                }
                case KeyCode::SHIFT:
                {
                    _highlight_mode = true;
                    return true;
                }
                default:
                {
                    if(!_non_rendering_char) {
                        return true;
                    }
                    return false;
                }
            }
        }
        case WindowsSystemMessage::KEYBOARD_CHAR:
        {
            if(IsClosed() || _non_rendering_char) {
                return false;
            }
            _entryline_changed = true;
            _entryline += static_cast<unsigned char>(wp);
            _cursor_position = std::end(_entryline);
            _selection_position = std::end(_entryline);
            return true;
        }
        case WindowsSystemMessage::KEYBOARD_KEYUP:
        {
            if(IsClosed()) {
                return false;
            }
            _entryline_changed = false;
            unsigned char char_code = static_cast<unsigned char>(wp);
            uint32_t lpBits = lp;
            //0bTPXRRRRESSSSSSSSCCCCCCCCCCCCCCCC
            //C: repeat count
            //S: scan code
            //E: extended key flag
            //R: reserved
            //X: context code: 1 if ALT is already down, 0 otherwise
            //P: previous state 1 for already down
            //T: transition state 1 if being released, 0 if being pressed
            constexpr uint32_t repeat_count_mask = 0b0000'0000'0000'0000'1111'1111'1111'1111; //0x0000FFFF;
            constexpr uint32_t scan_code_mask = 0b0000'0000'1111'1111'0000'0000'0000'0000; //0x00FF0000;
            constexpr uint32_t extended_key_mask = 0b0000'0001'0000'0000'0000'0000'0000'0000; //0x01000000;
            constexpr uint32_t reserved_mask = 0b0001'1110'0000'0000'0000'0000'0000'0000; //0x1E000000;
            constexpr uint32_t context_code_mask = 0b0010'0000'0000'0000'0000'0000'0000'0000; //0x20000000;
            constexpr uint32_t previous_state_mask = 0b0100'0000'0000'0000'0000'0000'0000'0000; //0x40000000;
            constexpr uint32_t transition_state_mask = 0b1000'0000'0000'0000'0000'0000'0000'0000; //0x80000000;
            bool is_extended_key = (lpBits & extended_key_mask) != 0;
            //bool is_alt_down = (lpBits & context_code_mask) != 0;
            auto my_key = InputSystem::ConvertWinVKToKeyCode(char_code);
            if(is_extended_key) {
                switch(my_key) {
                    case KeyCode::CTRL:
                    {
                        my_key = KeyCode::RCTRL;
                        break;
                    }
                    case KeyCode::SHIFT:
                    {
                        my_key = KeyCode::RSHIFT;
                        break;
                    }
                    case KeyCode::RETURN: //Numpad Enter
                    {
                        _output_changed = true;
                        break;
                    }
                    default: return false;
                }
            }
            switch(my_key) {
                case KeyCode::RSHIFT:
                {
                    _highlight_mode = false;
                    return true;
                }
                case KeyCode::RCTRL:
                {
                    _skip_nonwhitespace_mode = false;
                    return true;
                }
                case KeyCode::RETURN:
                {
                    _output_changed = true;
                    return true;
                }
                default:
                {
                    return false;
                }
            }
        }
        default:
        {
            return false;
        }
    }
}

void Console::RunCommand(std::string name_and_args) {
    if(name_and_args.empty()) {
        return;
    }
    name_and_args = StringUtils::TrimWhitespace(name_and_args);
    auto first_space = name_and_args.find_first_of(' ');
    std::string command = name_and_args.substr(0, first_space);
    std::string args = first_space == std::string::npos ? "" : name_and_args.substr(first_space);
    auto iter = _commands.find(command);
    if(iter == _commands.end()) {
        ErrorMsg("INVALID COMMAND");
        return;
    }
    iter->second.command_function(args);
}

void Console::RegisterCommand(const Command& command) {
    auto iter = _commands.find(command.command_name);
    if(iter == _commands.end()) {
        _commands.insert_or_assign(command.command_name, command);
    }
}

void Console::UnregisterCommand(const std::string& command_name) {
    auto iter = _commands.find(command_name);
    if(iter != _commands.end()) {
        _commands.erase(command_name);
    }
}

void Console::UnregisterAllCommands() {
    _commands.clear();
}

void Console::ToggleConsole() {
    _is_open = !_is_open;
}

bool Console::IsOpen() const {
    return _is_open;
}

bool Console::IsClosed() const {
    return !_is_open;
}

void Console::Open() {
    _is_open = true;
}

void Console::Close() {
    _is_open = false;
}

void Console::ToggleHighlightMode() {
    _highlight_mode = !_highlight_mode;
}

bool Console::IsHighlighting() const {
    return _highlight_mode;
}

void Console::PostEntryLine() {
    if(_entryline.empty()) {
        return;
    }
    PushEntrylineToOutputBuffer();
    RunCommand(_entryline);
    ClearEntryLine();
}

void Console::PushEntrylineToOutputBuffer() {
    PrintMsg(_entryline);
}

void Console::ClearEntryLine() {
    _entryline.clear();
    _cursor_position = std::begin(_entryline);
    _selection_position = std::begin(_entryline);
}

void Console::MoveCursorLeft(int distance /*= 1*/) {
    if(_cursor_position != _entryline.begin()) {
        if(!_highlight_mode) {
            if(std::distance(std::cbegin(_entryline), _cursor_position) > distance) {
                _cursor_position -= distance;
            } else {
                _cursor_position = std::begin(_entryline);
            }
            _selection_position = _cursor_position;
        } else {
            UpdateSelectedRange(-distance);
        }
    }
}

void Console::MoveCursorRight(int distance /*= 1*/) {
    if(_cursor_position != _entryline.end()) {
        if(!_highlight_mode) {
            if(std::distance(std::cend(_entryline), _cursor_position) < distance) {
                _cursor_position += distance;
            } else {
                _cursor_position = std::end(_entryline);
            }
            _selection_position = _cursor_position;
        } else {
            UpdateSelectedRange(distance);
        }
    }
}

void Console::UpdateSelectedRange(int distance) {
    if(distance > 0) {
        auto distance_from_end = std::distance(std::cend(_entryline), _cursor_position);
        if(distance_from_end > std::abs(distance)) {
            _cursor_position += distance;
        } else {
            _cursor_position = std::end(_entryline);
        }

        int direction = _cursor_position != std::end(_entryline) ? distance : 0;
        auto rangeStart = _cursor_position + direction;
        auto rangeEnd = _selection_position;
        if(!_highlight_mode && _selection_position < _cursor_position) {
            rangeStart = _selection_position;
            rangeEnd = _cursor_position;
        }
        _cursor_position = rangeStart;
        _selection_position = rangeEnd;
    } else if(distance < 0) {
        auto distance_from_beginning = std::distance(std::cbegin(_entryline), _cursor_position);
        if(distance_from_beginning > std::abs(distance)) {
            _cursor_position += distance;
        } else {
            _cursor_position = std::begin(_entryline);
        }

        int direction = _cursor_position != std::begin(_entryline) ? distance : 0;
        auto rangeStart = _cursor_position + direction;
        auto rangeEnd = _selection_position;
        if(!_highlight_mode && _selection_position < _cursor_position) {
            rangeStart = _selection_position;
            rangeEnd = _cursor_position;
        }
        _cursor_position = rangeStart;
        _selection_position = rangeEnd;
    }
}

void Console::Initialize() {
    _camera = new Camera2D;
    RegisterDefaultCommands();
}

void Console::RegisterDefaultCommands() {
    Console::Command help{};
    help.command_name = "help";
    help.help_text_short = "Displays every command with brief description.";
    help.help_text_long = help.help_text_short;
    help.command_function = [this](const std::string& /*args*/)->void {
        RunCommand("clear");
        for(auto& entry : _commands) {
            std::ostringstream ss;
            ss << entry.second.command_name << ": " << entry.second.help_text_short;
            PrintMsg(ss.str());
        }
    };
    RegisterCommand(help);

    //Console::Command echo{};
    //echo.command_name = "echo";
    //echo.help_text_short = "Displays command and arguments of entryline.";
    //echo.help_text_long = "echo [text]";
    //echo.command_function = [](const std::string& /*args*/)->void { /* DO NOTHING */ };
    //RegisterCommand(echo);

    Console::Command clear{};
    clear.command_name = "clear";
    clear.help_text_short = "Clears the output buffer.";
    clear.command_function = [this](const std::string& /*args*/)->void {
        _output_changed = true;
        _output_buffer.clear();
    };
    RegisterCommand(clear);
}

void Console::BeginFrame() {
    /* DO NOTHING */
}

void Console::Update(float deltaSeconds) {
    _current_blink_time += deltaSeconds;
    if(_blink_time < _current_blink_time) {
        _current_blink_time = 0.0f;
        _show_cursor = !_show_cursor;
    }
}

void Console::Render() const {

    if(IsClosed()) {
        return;
    }

    auto view_half_extents = SetupViewFromCamera();
    DrawBackground(view_half_extents);
    DrawOutput(view_half_extents);
    DrawEntryLine(view_half_extents);
    DrawCursor(view_half_extents);

}

void Console::DrawCursor(const Vector2& view_half_extents) const {
    if(!_show_cursor) {
        return;
    }
    float textline_bottom = view_half_extents.y * 0.99f;
    float textline_left = -view_half_extents.x;
    auto font = _renderer->GetFont("Arial32");
    auto first = _entryline.begin();
    auto has_text = !_entryline.empty();
    std::string text_left_of_cursor = has_text ? std::string(first, _cursor_position) : std::string("");
    float xPosOffsetToCaret = font->CalculateTextWidth(text_left_of_cursor);
    Matrix4 cursor_t = Matrix4::CreateTranslationMatrix(Vector3(textline_left + xPosOffsetToCaret, textline_bottom, 0.0f));
    Matrix4 model_cursor_mat = cursor_t;
    _renderer->SetModelMatrix(model_cursor_mat);
    _renderer->SetMaterial(font->GetMaterial());
    _renderer->DrawTextLine(font, "|", Rgba::WHITE);
}

void Console::DrawOutput(const Vector2& view_half_extents) const {
    if(!_output_changed) {
        return;
    }
    auto font = _renderer->GetFont("Arial32");
    _renderer->SetMaterial(font->GetMaterial());
    float y = font->GetLineHeight();
    std::size_t s = _output_buffer.size();
    for(std::size_t i = s - 1; i < s; --i) {
        auto entry = _output_buffer[i];
        Matrix4 cursor_t = Matrix4::CreateTranslationMatrix(Vector3(-view_half_extents.x, (view_half_extents.y * 0.99f) - (y * static_cast<float>(i + 1)), 0.0f));// -(static_cast<float>(s - 1) * y), 0.0f));
        Matrix4 model_cursor_mat = cursor_t;
        _renderer->SetModelMatrix(model_cursor_mat);
        _renderer->DrawTextLine(font, entry.str, entry.color);
    }
}

void Console::OutputMsg(const std::string& msg, const Rgba& color) {
    _output_changed = true;
    _output_buffer.push_back({msg, color});
}

void Console::PrintMsg(const std::string& msg) {
    OutputMsg(msg, Rgba::WHITE);
}

void Console::WarnMsg(const std::string& msg) {
    OutputMsg(msg, Rgba::YELLOW);
}

void Console::ErrorMsg(const std::string& msg) {
    OutputMsg(msg, Rgba::RED);
}

void Console::DrawBackground(const Vector2& view_half_extents) const {
    _renderer->SetModelMatrix(Matrix4::CreateScaleMatrix(view_half_extents * 2.0f));
    _renderer->SetMaterial(_renderer->GetMaterial("__2D"));
    _renderer->DrawQuad2D(Vector2::ZERO, Vector2::ONE * 0.5f, Rgba(0, 0, 0, 128));
}

void Console::DrawEntryLine(const Vector2& view_half_extents) const {
    auto font = _renderer->GetFont("Arial32");
    float textline_bottom = view_half_extents.y * 0.99f;
    float textline_left = -view_half_extents.x;
    //float textline_top = textline_bottom - font->GetLineHeight();

    Matrix4 entryline_t = Matrix4::CreateTranslationMatrix(Vector3(textline_left, textline_bottom, 0.0f));
    Matrix4 model_entryline_mat = entryline_t;
    _renderer->SetModelMatrix(model_entryline_mat);
    _renderer->SetMaterial(font->GetMaterial());

    if(_cursor_position != _selection_position) {
        //_renderer->DrawTextLine(font, std::string(_entryline, 0, std::distance(std::cbegin(_entryline), _cursor_position)), Rgba::WHITE);
        //_renderer->DrawTextLine(font, std::string(_entryline, std::distance(std::cbegin(_entryline), _selection_position), std::distance(_selection_position, std::cend(_entryline))), Rgba::WHITE);

        //float xPosOffsetToCaret = font->CalculateTextWidth(std::string(std::begin(_entryline), _cursor_position));
        //float xPosOffsetToSelect = font->CalculateTextWidth(std::string(std::begin(_entryline), _selection_position));
        //auto rangeStart = _cursor_position;
        //auto rangeEnd = _selection_position;
        //if(_selection_position < _cursor_position) {
        //    std::swap(rangeStart, rangeEnd);
        //    std::swap(xPosOffsetToCaret, xPosOffsetToSelect);
        //}

        //float xPosOffsetToStart = font->CalculateTextWidth(std::string(std::begin(_entryline), rangeStart));
        //Matrix4 highlight_t = Matrix4::CreateTranslationMatrix(Vector3(xPosOffsetToStart, 0.0f, 0.0f));
        //Matrix4 model_mat_highlight = highlight_t * model_entryline_mat;
        //_renderer->SetModelMatrix(model_mat_highlight);
        //_renderer->DrawTextLine(font, std::string(rangeStart, rangeEnd), Rgba::BLACK);

        //_renderer->SetMaterial(_renderer->GetMaterial("__unlit"));
        //_renderer->DrawQuad2D(xPosOffsetToCaret,
        //                      textline_bottom,
        //                      xPosOffsetToSelect,
        //                      textline_top,
        //                      Rgba(64, 64, 0, 255)
        //);

    } else {
        _renderer->DrawTextLine(font, _entryline, Rgba::WHITE);
    }
}

Vector2 Console::SetupViewFromCamera() const {
    const auto& window_dimensions = _renderer->GetOutput()->GetDimensions();
    float window_width = static_cast<float>(window_dimensions.x);
    float window_height = static_cast<float>(window_dimensions.y);
    float view_half_width = window_width * 0.50f;
    float view_half_height = window_height * 0.50f;

    Vector2 leftBottom = Vector2(-view_half_width, view_half_height);
    Vector2 rightTop = Vector2(view_half_width, -view_half_height);
    Vector2 nearFar = Vector2(0.0f, 1.0f);
    Vector2 cam_pos2 = Vector2(_camera->GetPosition());
    _camera->SetupView(leftBottom, rightTop, nearFar, MathUtils::M_16_BY_9_RATIO, -Vector3::Y_AXIS);

    _renderer->SetViewMatrix(_camera->GetViewMatrix());
    _renderer->SetProjectionMatrix(_camera->GetProjectionMatrix());

    return Vector2(view_half_width, view_half_height);
}

void Console::EndFrame() {
    /* DO NOTHING */
}
