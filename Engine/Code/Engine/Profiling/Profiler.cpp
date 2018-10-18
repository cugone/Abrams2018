#include "Engine/Profiling/Profiler.hpp"

#include "Engine/Core/ArgumentParser.hpp"
#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileLogger.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/Vertex3D.hpp"

#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Engine/RHI/RHIOutput.hpp"

#include <iomanip>
#include <sstream>

struct profiler_node_t {
    std::string tag_name{};
    decltype(TimeUtils::Now<FPMilliseconds>()) start_time{};
    decltype(TimeUtils::Now<FPMilliseconds>()) end_time{};
    profiler_node_t* parent = nullptr;
    std::vector<profiler_node_t*> children{};
    ~profiler_node_t() {
        if(parent) {
            auto me = std::find(std::begin(parent->children), std::end(parent->children), this);
            if(me != std::end(parent->children)) {
                *me = nullptr;
            }
        }
        parent = nullptr;
        for(auto& child : children) {
            delete child;
            child = nullptr;
        }
        children.clear();
        children.shrink_to_fit();
    }
};

struct profiler_data_t {
    std::string tag_name{};
    std::size_t callCount{};
    FPMilliseconds selfTime{};
    FPMilliseconds totalTime{};
    float selfRatio{};
    float totalRatio{};
    bool operator<(const profiler_data_t& rhs) const {
        return tag_name < rhs.tag_name;
    }
    friend std::ostream& operator<<(std::ostream& out_stream, const profiler_data_t& row) {
        out_stream << std::setw(60) << row.tag_name;
        out_stream << std::setw(10) << row.callCount;
        out_stream << std::setw(10) << std::fixed << std::setprecision(2) << row.totalRatio;
        out_stream << std::setw(20) << std::fixed << std::setprecision(4) << row.totalTime.count();
        out_stream << std::setw(10) << std::fixed << std::setprecision(2) << row.selfRatio;
        out_stream << std::setw(10) << std::fixed << std::setprecision(4) << row.selfTime.count();
        return out_stream;
    }
    std::string to_string() const {
        std::ostringstream ss;
        ss << *this;
        return ss.str();
    }
};

ProfilerReport::ProfilerReport(const Profiler* parent, profiler_node_t* frame, const ProfilerReportType& type /*= ProfilerReportType::Flat*/)
    : _parent(parent)
    , _current_frame(frame)
    , _report_type(type)
{
    /* DO NOTHING */
}

ProfilerReport::~ProfilerReport() {
    _current_frame = nullptr;
    _report_flat.clear();
    _report.clear();
    _report.shrink_to_fit();
}

void ProfilerReport::CreateView() {
    switch(_report_type) {
    case ProfilerReportType::Flat:
        CreateFlatView();
        break;
    case ProfilerReportType::Tree:
        CreateTreeView();
        break;
    default:
        break;
    }
}

void ProfilerReport::CreateTreeView() {
    /* DO NOTHING */
}

void ProfilerReport::CreateFlatView() {
    auto head = _current_frame;
    if(head) {
        auto data = profiler_data_t{};
        data.callCount = 1;
        data.tag_name = head->tag_name;
        auto cur_frame = head;
        if(cur_frame) {
            auto frame_time = cur_frame->end_time - cur_frame->start_time;
            data.selfTime = frame_time;
            for(const auto& f : cur_frame->children) {
                if(!f) {
                    continue;
                }
                frame_time = f->end_time - f->start_time;
                data.totalTime += frame_time;
            }
        }
        data.totalRatio = (head->end_time - head->start_time) / data.totalTime;
        data.selfRatio = data.selfTime / data.totalTime;
        auto inserted = _report_flat.insert(data);
        if(!inserted.second) {
            if(auto val = _report_flat.extract(inserted.first)) {
                ++val.value().callCount;
                _report_flat.insert(inserted.first, val.value());
            }
        }
    }
}

void ProfilerReport::SortByTotalTime() {
    std::sort(std::begin(_report), std::end(_report),
        [](const profiler_data_t& a, const profiler_data_t& b) {
            return a.totalTime < b.totalTime;
        }
    );
}

void ProfilerReport::Render(Renderer* renderer, std::vector<Vertex3D>& vbo, std::vector<unsigned int>& ibo) const {
    static const auto font = renderer->GetFont("System32");
    float draw_x = 0.0f;
    float draw_y = 0.0f;
    float font_height = font->GetLineHeight();
    Vector2 pos{ draw_x, draw_y };
    for(const auto& row : _report_flat) {
        pos.y += font_height;
        renderer->AppendMultiLineTextBuffer(font, row.to_string(), pos, CalculateRowColor(row), vbo, ibo);
    }
    renderer->SetMaterial(font->GetMaterial());
    renderer->DrawIndexed(PrimitiveType::Triangles, vbo, ibo);
}

void ProfilerReport::RenderHeader(Renderer* renderer, std::vector<Vertex3D>& vbo, std::vector<unsigned int>& ibo) {
    static const auto font = renderer->GetFont("System32");
    static const std::string header = ProfilerReport::CreateHeader();
    renderer->AppendMultiLineTextBuffer(font, header, Vector2::ZERO, Rgba::WHITE, vbo, ibo);
}

void ProfilerReport::Log(FileLogger* logger) const {
    if(_report.empty()) {
        logger->LogLine("No profile report to log.");
        return;
    }
    LogHeader(logger);
    for(const auto& row : _report) {
        LogRow(logger, row);
    }
}

int ProfilerReport::CalculateHeight() const {
    if(_current_frame) {
        return 1 + _current_frame->children.size();
    }
    return 0;
}

void ProfilerReport::AppendFrame(profiler_node_t* frame) {
    if(_current_frame == frame) {
        return;
    }
    auto last_frame = _current_frame;
    if(!last_frame->children.empty()) {
        frame->parent = last_frame;
        last_frame->children.push_back(frame);
    } else {
        frame->parent = last_frame;
        last_frame->children.push_back(frame);
        _current_frame = frame;
    }
}

void ProfilerReport::LogHeader(FileLogger* logger) const {
    logger->LogLine(CreateHeader());
}

void ProfilerReport::LogRow(FileLogger* logger, const profiler_data_t& row) const {
    logger->LogLine(row.to_string());
}

std::string ProfilerReport::CreateHeader() {
    std::ostringstream ss;
    ss << std::setw(60) << "TAG NAME";
    ss << std::setw(10) << "CALLS";
    ss << std::setw(10) << "TOTAL%";
    ss << std::setw(20) << "TOTAL TIME(ms)";
    ss << std::setw(10) << "SELF%";
    ss << std::setw(10) << "SELF TIME(ms)";
    return ss.str();
}

std::string ProfilerReport::CreateFlatReportString() const {
    std::ostringstream ss;
    if(_report.empty()) {
        ss << "No profile report.";
        return ss.str();
    }
    ss << CreateHeader() << '\n';
    for(const auto& row : _report) {
        ss << row << '\n';
    }
    return ss.str();
}

std::string ProfilerReport::CreateTreeReportString() const {
    return{};
}

Rgba ProfilerReport::CalculateRowColor(const profiler_data_t& row) const {
    float frame_time = row.selfTime.count();
    float fps = 1.0f / frame_time;
    if(fps >= 60.0f) {
        return Rgba::GREEN;
    } else if(fps > 30.0f) {
        return Rgba::YELLOW;
    } else if(fps > 15.0f) {
        return Rgba::ORANGE;
    } else {
        return Rgba::RED;
    }
}

Profiler::Profiler(Renderer* renderer, Console* console, FileLogger* logger)
    : EngineSubsystem()
    , _renderer(renderer)
    , _console(console)
    , _logger(logger)
    , _camera(new Camera2D)
{
    /* DO NOTHING */
}

Profiler::~Profiler() {
    PopAll();

    delete _camera;
    _renderer = nullptr;

    for(auto& node : _completedList) {
        DeleteTree(node);
        node = nullptr;
    }
    _completedList.clear();
    _completedList.shrink_to_fit();
    _activeNode = nullptr;

    _report->_parent = nullptr;
    delete _report;
    _report = nullptr;
}

void Profiler::Initialize() {
#ifdef PROFILE_BUILD
    RegisterCommands();
#endif
}

void Profiler::BeginFrame() {
#ifdef PROFILE_BUILD
#endif
}

void Profiler::Update(float /*deltaSeconds*/) {
#ifdef PROFILE_BUILD
#endif
}

void Profiler::Render() const {
#ifdef PROFILE_BUILD
    if(IsClosed()) {
        return;
    }

    _renderer->SetModelMatrix(Matrix4::GetIdentity());
    _renderer->SetViewMatrix(Matrix4::GetIdentity());
    _renderer->SetProjectionMatrix(Matrix4::GetIdentity());

    auto view_half_extents = SetupViewFromCamera();
    DrawBackground(view_half_extents);
    DrawOutput(view_half_extents);
#endif
}

void Profiler::EndFrame() {
#ifdef PROFILE_BUILD
    if(_updateTimer.CheckAndReset()) {
        FreeOldTrees();
        delete _report;
        _report = nullptr;
        SortByTagName();
        auto report = new ProfilerReport(this, GetPreviousFrame(), report_type);
        std::for_each(std::rbegin(_completedList), std::rend(_completedList) - 1, [&report](profiler_node_t* frame) {
            report->AppendFrame(frame);
        });
        report->CreateView();
        _report = report;
    }
#endif
}

bool Profiler::ProcessSystemMessage(const EngineMessage& /*msg*/) {
    return false;
}

Vector2 Profiler::SetupViewFromCamera() const {
    const auto& window = _renderer->GetOutput();
    const auto& window_dimensions = window->GetDimensions();
    const auto& aspect = window->GetAspectRatio();
    auto window_width = static_cast<float>(window_dimensions.x);
    auto window_height = static_cast<float>(window_dimensions.y);
    float view_half_width = window_width * 0.50f;
    float view_half_height = window_height * 0.50f;
    Vector2 leftBottom = Vector2(-view_half_width, view_half_height);
    Vector2 rightTop = Vector2(view_half_width, -view_half_height);
    Vector2 nearFar = Vector2(0.0f, 1.0f);
    _camera->SetupView(leftBottom, rightTop, nearFar, aspect);

    _renderer->SetViewMatrix(_camera->GetViewMatrix());
    _renderer->SetProjectionMatrix(_camera->GetProjectionMatrix());

    return Vector2(view_half_width, view_half_height);
}

void Profiler::DrawBackground(const Vector2& view_half_extents) const {
    _renderer->SetModelMatrix(Matrix4::CreateScaleMatrix(view_half_extents * 1.9f));
    _renderer->SetMaterial(_renderer->GetMaterial("__2D"));
    _renderer->DrawQuad2D(Rgba(0, 0, 0, 128));
}

void Profiler::DrawOutput(Vector2 view_half_extents) const {
    Vector2 top_left = Vector2{ -view_half_extents.x, -view_half_extents.y } *0.90f;
    Matrix4 T = Matrix4::CreateTranslationMatrix(top_left);
    Matrix4 R = Matrix4::GetIdentity();
    Matrix4 S = Matrix4::GetIdentity();
    Matrix4 M = T * R * S;
    _renderer->SetModelMatrix(M);
    const auto& font = _renderer->GetFont("System32");
    if(_completedList.empty()) {
        _renderer->DrawTextLine(font, "No timings captured.");
        return;
    }
    if(!_report) {
        _renderer->DrawTextLine(font, "No report generated.");
        return;
    }
    static std::vector<Vertex3D> vbo{};
    static std::vector<unsigned int> ibo{};
    vbo.clear();
    ibo.clear();
    ProfilerReport::RenderHeader(_renderer, vbo, ibo);
    float draw_y = font->GetLineHeight() * _report->CalculateHeight();
    T = T * Matrix4::CreateTranslationMatrix(Vector2{ 0.0f, draw_y });
    M = T * R * S;
    _renderer->SetModelMatrix(M);
    _report->Render(_renderer, vbo, ibo);
}

void Profiler::DeleteTree(profiler_node_t*& node) {
    for(auto& child : node->children) {
        DeleteTree(child);
    }
    node->parent = nullptr;
    node->children.clear();
    node->children.shrink_to_fit();
    delete node;
    node = nullptr;
}

void Profiler::FreeOldTrees() {
#ifdef PROFILE_BUILD
    if(_completedList.size() > MAX_PROFILE_TREES) {
        auto begin = std::begin(_completedList);
        auto end = std::end(_completedList) - MAX_PROFILE_TREES;
        std::for_each(begin, end,
        [this](profiler_node_t*& node) {
            delete node;
            node = nullptr;
        });
        _completedList.erase(std::remove(std::begin(_completedList), std::end(_completedList), nullptr), std::end(_completedList));
        return;
    }
#endif
}

void Profiler::SortByTagName() {
    std::sort(std::begin(_completedList), std::end(_completedList), [](const profiler_node_t* a, const profiler_node_t* b) { return a->tag_name < b->tag_name; });
}

void Profiler::Push(const std::string& tag_str) {
    if(_is_paused) {
        if(_unpause_requested && !_activeNode) {
            _is_paused = false;
            _snapshot_requested = false;
            _unpause_requested = false;
        }
        if(!_snapshot_requested) {
            return;
        }
    }
    if(_snapshot_requested) {
        _snapshot_requested = false;
    }
    auto node = new profiler_node_t;
    node->tag_name = tag_str;
    node->start_time = TimeUtils::Now<FPMilliseconds>();

    if(_activeNode) {
        node->parent = _activeNode;
        _activeNode->children.push_back(node);
    }
    _activeNode = node;
}

void Profiler::Pop() {
    if(!_activeNode) {
        return;
    }
    _activeNode->end_time = TimeUtils::Now<FPMilliseconds>();
    _completedList.push_back(_activeNode);
    _activeNode = _activeNode->parent;
}

void Profiler::PopAll() {
    while(_activeNode) {
        Pop();
    }
}

profiler_node_t* Profiler::GetPreviousFrame() const {
    if(_completedList.empty()) {
        return nullptr;
    }
    return _completedList.back();
}

profiler_node_t* Profiler::GetPreviousFrame(const std::string& root_tag) const {
    auto last_with_tag = std::find_if(_completedList.rbegin(), _completedList.rend(),
        [&](profiler_node_t* node) {
        return node->tag_name == root_tag;
    });
    if(last_with_tag == _completedList.rend()) {
        return nullptr;
    }
    return *last_with_tag;
}

void Profiler::DoMemstat() {
#ifdef PROFILE_BUILD
    if(!_console) {
        return;
    }
    _console->WarnMsg("Memory stats not yet implemented.");
#endif
}

void Profiler::DoPause() {
#ifdef PROFILE_BUILD
    _is_paused = true;
#endif
}

void Profiler::DoResume() {
#ifdef PROFILE_BUILD
    _unpause_requested = true;
    if(!_activeNode) {
        _is_paused = false;
    }
#endif
}

void Profiler::DoSnapshot() {
#ifdef PROFILE_BUILD
    _snapshot_requested = true;
#endif
}

void Profiler::DoFlatReport() {
#ifdef PROFILE_BUILD
    Open();
    ProfilerReport report(this, _activeNode);
    //std::vector<Vertex3D> vbo{};
    //std::vector<unsigned int> ibo{};
    //report.RenderHeader(_renderer, vbo, ibo);
    //report.Render(_renderer, vbo, ibo);
#endif
}

void Profiler::DoTreeReport() {
#ifdef PROFILE_BUILD
#endif
}

void Profiler::DoFlatLog() {
    #ifdef PROFILE_BUILD
    ProfilerReport report(this, GetPreviousFrame(), report_type);
    report.CreateView();
    report.Log(_logger);
    //std::vector<Vertex3D> vbo{};
    //std::vector<unsigned int> ibo{};
    //report.RenderHeader(_renderer, vbo, ibo);
    //report.Render(_renderer, vbo, ibo);
    #endif
}

void Profiler::DoTreeLog() {
    #ifdef PROFILE_BUILD
    #endif
}

bool Profiler::IsOpen() const {
    return _is_open;
}

bool Profiler::IsClosed() const {
    return !IsOpen();
}

void Profiler::Toggle() {
    _is_open = !_is_open;
}

void Profiler::Open() {
    _is_open = true;
}

void Profiler::Close() {
    _is_open = false;
}

void Profiler::RegisterCommands() {
#ifdef PROFILE_BUILD
    if(!_console) {
        return;
    }

    Console::Command memstat{};
    memstat.command_name = "memstat";
    memstat.help_text_short = "Show/Hide memory statistics.";
    memstat.help_text_long = memstat.help_text_short;
    memstat.command_function = [&](const std::string& /*args*/) {
        this->DoMemstat();
    };
    _console->RegisterCommand(memstat);

    Console::Command profiler{};
    profiler.command_name = "profiler";
    profiler.help_text_short = "Run profiler with options.";
    profiler.help_text_long = "profiler (pause|resume|snapshot|(report|log) (flat|tree)))";
    profiler.command_function = [this,&profiler](const std::string& args) {
        ArgumentParser arg_set(args);
        std::string opt{};
        if(arg_set.GetNext(opt)) {
            if(opt == "pause") {
                this->DoPause();
            } else if(opt == "resume") {
                this->DoResume();
            } else if(opt == "snapshot") {
                this->DoSnapshot();
            } else if(opt == "log") {
                std::string flat_or_tree = "flat";
                if(arg_set.GetNext(flat_or_tree)) {
                    if(flat_or_tree == "flat") {
                        this->DoFlatLog();
                    } else if(flat_or_tree == "tree") {
                        this->DoTreeLog();
                    } else {
                        _console->WarnMsg("Invalid argument: must be flat or tree");
                    }
                } else {
                    _console->WarnMsg("Missing argument: specify flat or tree");
                }
            } else if(opt == "report") {
                std::string flat_or_tree = "flat";
                if(arg_set.GetNext(flat_or_tree)) {
                    if(flat_or_tree == "flat") {
                        this->DoFlatReport();
                    } else if(flat_or_tree == "tree") {
                        this->DoTreeReport();
                    } else {
                        _console->WarnMsg("Invalid argument: must be flat or tree");
                    }
                } else {
                    _console->WarnMsg("Missing argument: specify flat or tree");
                }
            }
        } else {
            Toggle();
        }
    };
    _console->RegisterCommand(profiler);
#endif
}

void Profiler::UnregisterCommands() {
    _console->UnregisterCommand("memstat");
    _console->UnregisterCommand("profiler");
}
