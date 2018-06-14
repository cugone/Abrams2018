#pragma once

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Vertex3D.hpp"

#include "Engine/Math/Vector2.hpp"

#include <functional>
#include <map>
#include <string>
#include <vector>

class Camera2D;
class KerningFont;
class Renderer;

class Console : public EngineSubsystem {
public:
    struct Command {
        std::string command_name{};
        std::string help_text_short{};
        std::string help_text_long{};
        std::function<void(const std::string& args)> command_function{};
    };
    Console(Renderer* renderer);
    virtual ~Console();

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

    void RunCommand(std::string name_and_args);
    void RegisterCommand(const Console::Command& command);
    void UnregisterCommand(const std::string& command_name);

    void PrintMsg(const std::string& msg);
    void WarnMsg(const std::string& msg);
    void ErrorMsg(const std::string& msg);

    void* GetAcceleratorTable() const;
protected:
private:
    struct OutputEntry {
        std::string str{};
        Rgba color = Rgba::WHITE;
    };
    void PostEntryLine();
    void PushEntrylineToOutputBuffer();
    void PushEntrylineToBuffer();
    void ClearEntryLine();
    void MoveCursorLeft(int distance = 1);
    void MoveCursorRight(int distance = 1);
    void MoveCursorToEnd();
    void MoveCursorToFront();
    void UpdateSelectedRange(int distance);

    bool HandleLeftKey();
    bool HandleRightKey();
    bool HandleDelKey();
    bool HandleHomeKey();
    bool HandleEndKey();
    bool HandleTildeKey();
    bool HandleReturnKey();
    bool HandleUpKey();
    bool HandleDownKey();
    bool HandleBackspaceKey();
    bool HandleEscapeKey();
    bool HandleTabKey();
    bool HandleClipboardCopy();
    void HandleClipboardPaste();
    void HandleClipboardCut();

    void HistoryUp();
    void HistoryDown();

    void InsertCharInEntryLine(unsigned char c);
    void PopConsoleBuffer();
    void RemoveTextInFrontOfCaret();
    void RemoveTextBehindCaret();
    void RemoveText(std::string::const_iterator start, std::string::const_iterator end);
    std::string CopyText(std::string::const_iterator start, std::string::const_iterator end);
    void PasteText(const std::string& text, std::string::const_iterator loc);
    void DrawBackground(const Vector2& view_half_extents) const;
    void DrawEntryLine(const Vector2& view_half_extents) const;
    void DrawCursor(const Vector2& view_half_extents) const;
    void DrawOutput(const Vector2& view_half_extents) const;
    void BuildOutputBuffer(KerningFont* font, const std::string& text, const Vector2& start_position, const Rgba& color, std::vector<Vertex3D>& vbo, std::vector<unsigned int>& ibo) const;

    void OutputMsg(const std::string& msg, const Rgba& color);

    void RegisterDefaultCommands();
    void RegisterDefaultFont();
    void UnregisterAllCommands();

    void ToggleConsole();
    bool IsOpen() const;
    bool IsClosed() const;
    void Open();
    void Close();

    void ToggleHighlightMode();
    void SetHighlightMode(bool value);
    bool IsHighlighting() const;
    void SetOutputChanged(bool value);
    void SetSkipNonWhitespaceMode(bool value);

    void AutoCompleteEntryline();

    Vector2 SetupViewFromCamera() const;

    Renderer* _renderer = nullptr;
    Camera2D* _camera = nullptr;
    std::map<std::string, Console::Command> _commands{};
    std::vector<std::string> _entryline_buffer{};
    std::vector<OutputEntry> _output_buffer{};
    std::string _entryline{};
    std::string::const_iterator _cursor_position{};
    std::string::const_iterator _selection_position{};
    decltype(_entryline_buffer)::const_iterator _current_history_position{};
    float _default_blink_time = 0.33f;
    float _blink_time = _default_blink_time;
    float _current_blink_time = 0.0f;
    bool _show_cursor = false;
    bool _is_open = false;
    bool _highlight_mode = false;
    bool _skip_nonwhitespace_mode = false;
    bool _dirty_text = false;
    bool _non_rendering_char = false;
    bool _entryline_changed = false;
    bool _output_changed = false;

};
