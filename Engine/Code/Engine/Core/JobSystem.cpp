#include "Engine/Core/JobSystem.hpp"

#include "Engine/Core/TimeUtils.hpp"

#include <chrono>

std::vector<ThreadSafeQueue<Job*>*> JobSystem::_queues = std::vector<ThreadSafeQueue<Job*>*>{};
std::vector<std::condition_variable*> JobSystem::_signals = std::vector<std::condition_variable*>{};

void JobSystem::GenericJobWorker(std::condition_variable* signal) {
    JobConsumer jc;
    jc.add_category(JobType::GENERIC);
    this->SetCategorySignal(JobType::GENERIC, signal);
    while(_is_running) {
        std::unique_lock<std::mutex> _lock(_generic_mutex);
        //wait if no jobs were consumed
        signal->wait(_lock, [&jc]()->bool { return jc.consume_all(); });
    }
    jc.consume_all();
}

void JobConsumer::add_category(const JobType& category) {
    auto q = JobSystem::_queues[static_cast<std::underlying_type_t<JobType>>(category)];
    if(q) {
        _consumables.push_back(q);
    }
}

bool JobConsumer::consume_job() {
    if(_consumables.empty()) {
        return false;
    }
    for(auto& consumable : _consumables) {
        auto& queue = *consumable;
        if(queue.empty()) {
            return false;
        }
        Job* job = queue.front();
        queue.pop();
        job->work_cb(job->user_data);
        job->OnFinish();
        job->state = JobState::FINISHED;
        delete job;
    }
    return true;
}

unsigned int JobConsumer::consume_all() {
    unsigned int processed_jobs = 0;
    while(consume_job()) {
        ++processed_jobs;
    }
    return processed_jobs;
}

void JobConsumer::consume_for_ms(unsigned int ms) {
    using milliseconds = std::chrono::duration<unsigned int, std::milli>;
    using clock = std::chrono::high_resolution_clock;
    milliseconds ms_t = milliseconds(ms);
    auto start_time = clock::now();
    auto end_time = clock::now();
    while(std::chrono::duration_cast<milliseconds>(end_time - start_time).count() < ms_t.count() && consume_job()) {
        end_time = clock::now();
    }
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
    _signals[static_cast<std::underlying_type_t<JobType>>(JobType::GENERIC)] = new std::condition_variable;

    _generic_consumer = new JobConsumer;
    _generic_consumer->add_category(JobType::GENERIC);

    std::thread generic_thread(&JobSystem::GenericJobWorker, this, _signals[static_cast<std::underlying_type_t<JobType>>(JobType::GENERIC)]);
    generic_thread.detach();
    for(int i = 0; i < core_count; ++i) {
        std::thread t(&JobSystem::GenericJobWorker, this, _signals[static_cast<std::underlying_type_t<JobType>>(JobType::GENERIC)]);
        t.detach();
    }

}

void JobSystem::BeginFrame() {
    MainStep();
}

void JobSystem::Shutdown() {
    _is_running = false;
    _main_job_signal = nullptr;
    delete _generic_consumer;
    _generic_consumer = nullptr;

    for(std::size_t i = 0; i < static_cast<std::underlying_type_t<JobType>>(JobType::MAX); ++i) {
        if(_signals[i] == nullptr) {
            continue;
        }
        _signals[i]->notify_all();
        while(!_queues[i]->empty()) {
            Job* job = _queues[i]->front();
            _queues[i]->pop();
            delete job;
            job = nullptr;
        }
        delete _signals[i];
        _signals[i] = nullptr;
    }

    _queues.clear();
    _signals.clear();
}

void JobSystem::MainStep() {
    JobConsumer jc;
    jc.add_category(JobType::MAIN);
    SetCategorySignal(JobType::MAIN, _main_job_signal);
    jc.consume_all();
}

void JobSystem::SetCategorySignal(const JobType& category_id, std::condition_variable* signal) {
    _signals[static_cast<std::underlying_type_t<JobType>>(category_id)] = signal;
}

Job* JobSystem::Create(const JobType& category, const std::function<void(void*)>& cb, void* user_data) {
    auto j = new Job(*this);
    j->type = category;
    j->state = JobState::CREATED;
    j->work_cb = cb;
    j->user_data = user_data;
    j->num_dependencies = 1;
    return std::move(j);
}

void JobSystem::Run(const JobType& category, const std::function<void(void*)>& cb, void* user_data) {
    Job* job = Create(category, cb, user_data);
    job->state = JobState::RUNNING;
    DispatchAndRelease(job);
}

void JobSystem::Dispatch(Job* job) {
    job->state = JobState::DISPATCHED;
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
    while(job->state != JobState::FINISHED) {
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
    parent->AddDependent(parent);
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
    dependents.push_back(dependent);
}
