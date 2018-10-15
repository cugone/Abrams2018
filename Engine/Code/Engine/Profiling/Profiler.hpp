#pragma once

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Stopwatch.hpp"

#include <set>
#include <string>
#include <vector>

class Renderer;
class Camera2D;
class Console;
class Vector2;
class FileLogger;
class ProfilerReport;

struct profiler_node_t;
struct profiler_data_t;

enum class ProfilerReportType {
    Flat,
    Tree,
};

class Profiler : public EngineSubsystem {
public:
    Profiler(Renderer* renderer, Console* console);
    Profiler(Renderer* renderer, Console* console, const std::string& tag_str);
    virtual ~Profiler();

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(float) override;
    virtual void Render() const override;
    virtual void EndFrame() override;

    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;
    void Push(const std::string& tag_str);
    void Pop();

    profiler_node_t* GetPreviousFrame() const;
    profiler_node_t* GetPreviousFrame(const std::string& root_tag) const;
    void SetReportType(const ProfilerReportType& type);
    void SetLastReport(ProfilerReport* report);
protected:
private:
    bool IsOpen() const;
    bool IsClosed() const;
    void Toggle();
    void Open();
    void Close();

    void RegisterCommands();
    void UnregisterCommands();

    void DoMemstat();
    void DoPause();
    void DoResume();
    void DoSnapshot();
    void DoFlatReport();
    void DoTreeReport();

    void FreeOldTrees();
    void DeleteTree(profiler_node_t*& head);


    Vector2 SetupViewFromCamera() const;
    void DrawBackground(const Vector2& view_half_extents) const;
    void DrawOutput(Vector2 view_half_extents) const;

    Renderer* _renderer = nullptr;
    Console* _console = nullptr;
    Camera2D* _camera = nullptr;

    profiler_node_t* _activeNode = nullptr;
    std::vector<profiler_node_t*> _completedList{};

    ProfilerReport* _last_report = nullptr;

    Stopwatch _updateTimer{};
    ProfilerReportType _report_type = ProfilerReportType::Flat;

    uint8_t _is_running: 1;
    uint8_t _is_open: 1;
    uint8_t _is_paused: 1;
    uint8_t _snapshot_requested: 1;
    uint8_t _pause_requested: 1;
    uint8_t _unpause_requested: 1;
    uint8_t _bitfield_dummy2: 1;
    uint8_t _bitfield_dummy3: 1;

    friend class ProfilerReport;
};


class ProfilerReport {
public:
    explicit ProfilerReport(const Profiler* parent, profiler_node_t* frame, const ProfilerReportType& type = ProfilerReportType::Flat);
    ~ProfilerReport();

    void CreateTreeView();
    void CreateFlatView();
    void SortByTotalTime();
    void Render(Renderer* renderer) const;
    void Log(FileLogger* logger) const;

protected:
private:
    void LogHeader(FileLogger* logger) const;
    void LogRow(FileLogger* logger, const  profiler_data_t* row) const;
    std::string CreateHeader() const;
    std::string CreateFlatReportString() const;
    std::string CreateTreeReportString() const;

    Rgba CalculateRowColor(const profiler_data_t* row) const;

    const Profiler* _parent = nullptr;
    profiler_node_t* _current_frame = nullptr;
    ProfilerReportType _report_type = ProfilerReportType::Flat;
    std::vector<profiler_data_t*> _report{};
    std::set<profiler_data_t*> _report_flat{};
    friend class Profiler;
};


#if defined PROFILE_FUNCTION
    #undef PROFILE_FUNCTION
#endif

#ifdef PROFILE_BUILD
#define PROFILE_FUNCTION_START(profiler_ptr) \
if((profiler_ptr)) { \
    (profiler_ptr)->Push(__FUNCTION__); \
}

#define PROFILE_FUNCTION_END(profiler_ptr) \
if((profiler_ptr)) { \
    (profiler_ptr)->Pop(); \
    ProfilerReport TOKEN_PASTE(__preport_, __LINE__)((profiler_ptr), (profiler_ptr)->GetPreviousFrame()); \
    ProfilerReport* pr = new ProfilerReport((profiler_ptr), (profiler_ptr)->GetPreviousFrame()); \
    pr->CreateFlatView(); \
    (profiler_ptr)->SetLastReport(pr); \
}

#else
#define PROFILE_FUNCTION(profiler_ptr)
#define PROFILE_FUNCTION_START(profiler_ptr)
#define PROFILE_FUNCTION_END(profiler_ptr)
#endif