#include "Engine/Core/JobSystem.hpp"

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/Win.hpp"

#include <chrono>

std::vector<ThreadSafeQueue<Job*>*> JobSystem::_queues = std::vector<ThreadSafeQueue<Job*>*>{};
std::vector<std::condition_variable*> JobSystem::_signals = std::vector<std::condition_variable*>{};

void JobSystem::GenericJobWorker(std::condition_variable* signal) {
    JobConsumer jc;
    jc.AddCategory(JobType::Generic);
    this->SetCategorySignal(JobType::Generic, signal);
    while(IsRunning()) {
        if(signal) {
            std::unique_lock<std::mutex> _lock(_cs);
            signal->wait(_lock);
            if(!_is_running) {
                break;
            }
            jc.ConsumeAll();
        }
    }
    jc.ConsumeAll();
}

void JobConsumer::AddCategory(const JobType& category) {
    auto categoryAsSizeT = static_cast<std::underlying_type_t<JobType>>(category);
    if(categoryAsSizeT >= JobSystem::_queues.size()) {
        return;
    }
    auto q = JobSystem::_queues[categoryAsSizeT];
    if(q) {
        _consumables.push_back(q);
    }
}

bool JobConsumer::ConsumeJob() {
    if(_consumables.empty()) {
        return false;
    }
    for(auto& consumable : _consumables) {
        if(!consumable) {
            continue;
        }
        auto& queue = *consumable;
        if(queue.empty()) {
            return false;
        }
        auto job = queue.front();
        queue.pop();
        job->work_cb(job->user_data);
        job->OnFinish();
        job->state = JobState::Finished;
        delete job;
    }
    return true;
}

unsigned int JobConsumer::ConsumeAll() {
    unsigned int processed_jobs = 0;
    while(ConsumeJob()) {
        ++processed_jobs;
    }
    return processed_jobs;
}

void JobConsumer::ConsumeForMs(unsigned int ms) {
    using milliseconds = std::chrono::duration<unsigned int, std::milli>;
    using clock = std::chrono::high_resolution_clock;
    milliseconds ms_t = milliseconds(ms);
    auto start_time = clock::now();
    auto end_time = clock::now();
    while(std::chrono::duration_cast<milliseconds>(end_time - start_time).count() < ms_t.count())
    {
        ConsumeJob();
        end_time = clock::now();
    }
}

bool JobConsumer::HasJobs() const {
    if(_consumables.empty()) {
        return false;
    }
    bool has_job = false;
    for(auto& consumable : _consumables) {
        auto& queue = *consumable;
        if(!queue.empty()) {
            has_job |= true;
        }
    }
    return has_job;
}

JobSystem::~JobSystem() {
    Shutdown();
}

void JobSystem::Initialize(int genericCount, std::size_t categoryCount, std::condition_variable* mainJobSignal) {
    int core_count = static_cast<int>(std::thread::hardware_concurrency());
    if(genericCount <= 0) {
        core_count += genericCount;
    }
    --core_count;
    _main_job_signal = mainJobSignal;
    _queues.resize(categoryCount);
    _signals.resize(categoryCount);
    _is_running = true;

    for(std::size_t i = 0; i < categoryCount; ++i) {
        _queues[i] = new ThreadSafeQueue<Job*>{};
    }

    for(std::size_t i = 0; i < categoryCount; ++i) {
        _signals[i] = nullptr;
    }
    _signals[static_cast<std::underlying_type_t<JobType>>(JobType::Generic)] = new std::condition_variable;

    for(int i = 0; i < core_count; ++i) {
        std::thread t(&JobSystem::GenericJobWorker, this, _signals[static_cast<std::underlying_type_t<JobType>>(JobType::Generic)]);
        std::wstring name = L"Generic Job Thread ";
        name += std::to_wstring(i + 1);
        ::SetThreadDescription(t.native_handle(), name.c_str());
        t.detach();
    }
}

void JobSystem::BeginFrame() {
    MainStep();
}

void JobSystem::Shutdown() {
    if(!IsRunning()) {
        return;
    }
    _is_running = false;
    auto max_jobtype = static_cast<std::underlying_type_t<JobType>>(JobType::Max);
    for(std::size_t i = 0; i < max_jobtype; ++i) {
        if(_signals[i] == nullptr) {
            continue;
        }
        _signals[i]->notify_all();
        while(!_queues[i]->empty()) {
                Job* job = _queues[i]->front();
                if(job->state == JobState::Finished) {
                    _queues[i]->pop();
                    delete job;
                    job = nullptr;
                }
        }
        delete _queues[i];
        _queues[i] = nullptr;
        delete _signals[i];
        _signals[i] = nullptr;
    }
    _main_job_signal = nullptr;

    _queues.clear();
    _queues.shrink_to_fit();

    _signals.clear();
    _signals.shrink_to_fit();
}

void JobSystem::MainStep() {
    JobConsumer jc;
    jc.AddCategory(JobType::Main);
    SetCategorySignal(JobType::Main, _main_job_signal);
    jc.ConsumeAll();
}

void JobSystem::SetCategorySignal(const JobType& category_id, std::condition_variable* signal) {
    _signals[static_cast<std::underlying_type_t<JobType>>(category_id)] = signal;
}

Job* JobSystem::Create(const JobType& category, const std::function<void(void*)>& cb, void* user_data) {
    auto j = new Job(*this);
    j->type = category;
    j->state = JobState::Created;
    j->work_cb = cb;
    j->user_data = user_data;
    j->num_dependencies = 1;
    return std::move(j);
}

void JobSystem::Run(const JobType& category, const std::function<void(void*)>& cb, void* user_data) {
    Job* job = Create(category, cb, user_data);
    job->state = JobState::Running;
    DispatchAndRelease(job);
}

void JobSystem::Dispatch(Job* job) {
    job->state = JobState::Dispatched;
    ++job->num_dependencies;
    auto jobtype = static_cast<std::underlying_type_t<JobType>>(job->type);
    _queues[jobtype]->push(job);
    auto signal = _signals[jobtype];
    if(signal) {
        signal->notify_all();
    }
}

bool JobSystem::Release(Job* job) {
    auto dcount = --job->num_dependencies;
    if(dcount != 0) {
        return false;
    }
    delete job;
    return true;
}

void JobSystem::Wait(Job* job) {
    while(job->state != JobState::Finished) {
        std::this_thread::yield();
    }
}

void JobSystem::DispatchAndRelease(Job* job) {
    Dispatch(job);
    Release(job);
}

void JobSystem::WaitAndRelease(Job* job) {
    Wait(job);
    Release(job);
}

bool JobSystem::IsRunning() {
    bool running = false;
    {
        std::scoped_lock<std::mutex> lock(_cs);
        running = _is_running;
    }
    return running;
}

void JobSystem::SetIsRunning(bool value /*= true*/) {
    std::scoped_lock<std::mutex> lock(_cs);
    _is_running = value;
}

Job::Job(JobSystem& jobSystem)
    : _job_system(&jobSystem)
{
    /* DO NOTHING */
}

Job::~Job() {
    delete user_data;
}

void Job::DependsOn(Job* dependency) {
    this->DependentOn(dependency);
}

void Job::DependentOn(Job* parent) {
    parent->AddDependent(this);
}

void Job::OnDependancyFinished() {
    ++num_dependencies;
    _job_system->DispatchAndRelease(this);
}

void Job::OnFinish() {
    for(auto& dependent : dependents) {
        dependent->OnDependancyFinished();
    }
}

void Job::AddDependent(Job* dependent) {
    dependent->state = JobState::Enqueued;
    dependents.push_back(dependent);
}
