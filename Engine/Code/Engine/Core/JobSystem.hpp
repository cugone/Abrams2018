#pragma once

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/ThreadSafeQueue.hpp"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <vector>

class Job;
class JobSystem;

enum class JobType : std::size_t {
    Generic,
    Logging,
    Io,
    Render,
    Main,
    Max,
};

enum class JobState : unsigned int {
    None,
    Created,
    Dispatched,
    Enqueued,
    Running,
    Finished,
    Max,
};

class Job {
public:
    Job(JobSystem& jobSystem);
    ~Job();
    JobType type{};
    JobState state{};
    std::function<void(void*)> work_cb;
    void* user_data;

    void DependsOn(Job* dependency);
    void DependentOn(Job* parent);
    void OnDependancyFinished();
    void OnFinish();

    std::vector<Job*> dependents{};
    std::atomic<unsigned int> num_dependencies{ 0u };
private:
    void AddDependent(Job* dependent);
    JobSystem* _job_system = nullptr;
};

class JobConsumer {
public:
    void AddCategory(const JobType& category);
    bool ConsumeJob();
    unsigned int ConsumeAll();
    void ConsumeForMs(unsigned int ms);
    bool HasJobs() const;
private:
    std::vector<ThreadSafeQueue<Job*>*> _consumables{};
    friend class JobSystem;
};


class JobSystem {
public:
    JobSystem() = default;
    ~JobSystem();

    void Initialize(int genericCount, std::size_t categoryCount, std::condition_variable* mainJobSignal);
    void BeginFrame();
    void Shutdown();

    void SetCategorySignal(const JobType& category_id, std::condition_variable* signal);
    Job* Create(const JobType& category, const std::function<void(void*)>& cb, void* user_data);
    void Run(const JobType& category, const std::function<void(void*)>& cb, void* user_data);
    void Dispatch(Job* job);
    bool Release(Job* job);
    void Wait(Job* job);
    void DispatchAndRelease(Job* job);
    void WaitAndRelease(Job* job);
    bool IsRunning();
    void SetIsRunning(bool value = true);
protected:
private:
    void MainStep();
    void GenericJobWorker(std::condition_variable* signal);

    static std::vector<ThreadSafeQueue<Job*>*> _queues;
    static std::vector<std::condition_variable*> _signals;
    std::thread _generic_worker{};
    std::condition_variable* _main_job_signal = nullptr;
    std::mutex _cs{};
    bool _is_running = false;
    friend class JobConsumer;
};