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
    profiler_node_t* child = nullptr;
    profiler_node_t* next = nullptr;
    profiler_node_t* previous = nullptr;
    bool operator<(const profiler_node_t& rhs) {
        return tag_name < rhs.tag_name;
    }
};

struct profiler_data_t {
    std::string tag_name{};
    std::size_t callCount{};
    FPMilliseconds selfTime{};
    FPMilliseconds totalTime{};
    float selfRatio{};
    float totalRatio{};
    bool operator<(const profiler_data_t& rhs) {
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
    if(_parent) {
        Render(_parent->_renderer);
    }
    _current_frame = nullptr;
    for(auto& row : _report) {
        delete row;
        row = nullptr;
    }
    _report.clear();
    _report.shrink_to_fit();
}

void ProfilerReport::CreateTreeView() {

}

void ProfilerReport::CreateFlatView() {
    auto head = _current_frame;
    while(head) {
        auto data = new profiler_data_t;
        data->callCount = 1;
        data->tag_name = head->tag_name;
        auto cur_frame = head;
        while(cur_frame) {
            auto frame_time = cur_frame->end_time - cur_frame->start_time;
            data->selfTime = frame_time;
            data->totalTime += frame_time;
            cur_frame = cur_frame->child;
        }
        data->totalRatio = (head->end_time - head->start_time) / data->totalTime;
        data->selfRatio = data->selfTime / data->totalTime;
        auto inserted = _report_flat.insert(data);
        if(!inserted.second) {
            ++data->callCount;
        }
        head = head->child;
    }
}

void ProfilerReport::SortByTotalTime() {
    std::sort(std::begin(_report), std::end(_report),
        [](const profiler_data_t* a, const profiler_data_t* b) {
            return a->totalTime < b->totalTime;
        }
    );
}

void ProfilerReport::Render(Renderer* renderer) const {
    static const auto font = renderer->GetFont("System32");
    static const std::string header = CreateHeader();
    std::vector<Vertex3D> vbo{};
    std::vector<unsigned int> ibo{};
    float draw_x = 0.0f;
    float draw_y = 0.0f;
    float font_height = font->GetLineHeight();
    Vector2 pos{ draw_x, draw_y };
    renderer->AppendMultiLineTextBuffer(font, header, pos, Rgba::WHITE, vbo, ibo);
    for(auto row : _report_flat) {
        pos.y += font_height;
        renderer->AppendMultiLineTextBuffer(font, row->to_string(), pos, CalculateRowColor(row), vbo, ibo);
    }
    renderer->SetMaterial(font->GetMaterial());
    renderer->DrawIndexed(PrimitiveType::Triangles, vbo, ibo);
}

void ProfilerReport::Log(FileLogger* logger) const {
    if(_report.empty()) {
        logger->LogLine("No profile report to log.");
        return;
    }
    LogHeader(logger);
    for(const auto row : _report) {
        LogRow(logger, row);
    }
}

void ProfilerReport::LogHeader(FileLogger* logger) const {
    logger->LogLine(CreateHeader());
}

void ProfilerReport::LogRow(FileLogger* logger, const profiler_data_t* row) const {
    logger->LogLine(row->to_string());
}

std::string ProfilerReport::CreateHeader() const {
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
    for(const auto row : _report) {
        ss << *row << '\n';
    }
    return ss.str();
}

std::string ProfilerReport::CreateTreeReportString() const {
    return{};
}

Rgba ProfilerReport::CalculateRowColor(const profiler_data_t* row) const {
    float frame_time = row->selfTime.count();
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

Profiler::Profiler(Renderer* renderer, Console* console, const std::string& tag_str)
    : EngineSubsystem()
    , _renderer(renderer)
    , _console(console)
    , _camera(new Camera2D)
{
    _updateTimer.SetSeconds(2.0f);
    Push(tag_str);
}

Profiler::Profiler(Renderer* renderer, Console* console)
    : EngineSubsystem()
    , _renderer(renderer)
    , _console(console)
    , _camera(new Camera2D)
{
    _updateTimer.SetSeconds(2.0);
    /* DO NOTHING */
}

Profiler::~Profiler() {
    Pop();

    delete _camera;
    _renderer = nullptr;

    for(auto& node : _completedList) {
        DeleteTree(node);
        node = nullptr;
    }
    _completedList.clear();
    _completedList.shrink_to_fit();
    _activeNode = nullptr;

    if(_last_report) {
        _last_report->_parent = nullptr;
    }
    delete _last_report;
    _last_report = nullptr;
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
        delete _last_report;
        _last_report = new ProfilerReport(this, GetPreviousFrame(), _report_type);
        _last_report->CreateFlatView();
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

    auto frame = GetPreviousFrame();
    if(!frame) {
        _renderer->DrawTextLine(_renderer->GetFont("System32"), "Bad Frame Request.");
        return;
    }

    if(_last_report) {
        _last_report->CreateFlatView();
        _last_report->SortByTotalTime();
        _last_report->Render(_renderer);
    }
}

void Profiler::FreeOldTrees() {
    if(_completedList.size() > MAX_PROFILE_TREES) {
        auto end = std::end(_completedList) - MAX_PROFILE_TREES - 1;
        auto begin = std::begin(_completedList);
        std::for_each(begin, end,
        [this](profiler_node_t*& node) {
            DeleteTree(node);
            node = nullptr;
        });
        _completedList.erase(std::remove(std::begin(_completedList), std::end(_completedList), nullptr), std::end(_completedList));
    }
}

void Profiler::DeleteTree(profiler_node_t*& head) {
    if(head == nullptr) return;
    if(head->child) {
        DeleteTree(head->child);
        head->child = nullptr;
    }
    profiler_node_t* last_node = head->previous;
    if(last_node && last_node->child) {
        DeleteTree(last_node->child);
        last_node->child = nullptr;
    }
    while(last_node && last_node->child == nullptr) {
        auto cur_node = last_node;
        last_node = last_node->previous;
        if(last_node == cur_node) {
            break;
        }
        delete cur_node;
        cur_node = nullptr;
    }
    delete head;
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
    node->parent = _activeNode;

    if(_activeNode) {
        if(_activeNode->child) {
            _activeNode->child->previous->next = node;
            node->previous = _activeNode->child->previous;
            _activeNode->child->previous = node;
        } else {
            _activeNode->child = node;
            _activeNode->child->previous = node;
            _activeNode->child->next = node;
        }
    }
    _activeNode = node;
}

void Profiler::Pop() {
    if(!_activeNode) {
        return;
    }
    _activeNode->end_time = TimeUtils::Now<FPMilliseconds>();
    if(_activeNode->parent) {
        _activeNode = _activeNode->parent;
    } else {
        _completedList.push_back(_activeNode);
        _activeNode = nullptr;
        FreeOldTrees();
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

void Profiler::SetReportType(const ProfilerReportType& type) {
    _report_type = type;
}

void Profiler::SetLastReport(ProfilerReport* report) {
    delete _last_report;
    _last_report = report;
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
    report.Render(_renderer);
#endif
}

void Profiler::DoTreeReport() {
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
    profiler.help_text_long = "profiler (pause|resume|snapshot|(report (flat|tree)))";
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
