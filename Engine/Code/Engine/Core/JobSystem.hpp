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
    GENERIC,
    MAIN,
    IO,
    RENDER,
    LOGGING,
    MAX,
};

enum class JobState : unsigned int {
    NONE,
    CREATED,
    DISPATCHED,
    ENQUEUED,
    RUNNING,
    FINISHED,
    MAX,
};

class Job {
public:
    Job(JobSystem& jobSystem);
    ~Job();
    JobType type = JobType::GENERIC;
    JobState state = JobState::NONE;
    std::function<void(void*)> work_cb;
    void* user_data;

    void DependsOn(Job* dependency);
    void DependentOn(Job* parent);
    void OnDependancyFinished();
    void OnFinish();

    std::vector<Job*> dependents;
    std::atomic<unsigned int> num_dependencies;
private:
    void AddDependent(Job* dependent);
    JobSystem* _job_system = nullptr;
};

class JobConsumer {
public:
    void add_category(const JobType& category);
    bool consume_job();
    unsigned int consume_all();
    void consume_for_ms(unsigned int ms);

private:
    std::vector<ThreadSafeQueue<Job*>*> _consumables{};
    friend class JobSystem;
};


class JobSystem : public EngineSubsystem {
public:
    JobSystem(int genericCount, std::size_t categoryCount, std::condition_variable* mainJobSignal);
    virtual ~JobSystem();

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

    void SetCategorySignal(const JobType& category_id, std::condition_variable* signal);
    Job* Create(const JobType& category, const std::function<void(void*)>& cb, void* user_data);
    void Run(const JobType& category, const std::function<void(void*)>& cb, void* user_data);
    void Dispatch(Job* job);
    bool Release(Job* job);
    void Wait(Job* job);
    void DispatchAndRelease(Job* job);
    void WaitAndRelease(Job* job);

protected:
private:
    void MainStep();
    void GenericJobWorker(std::condition_variable* signal);

    static std::vector<ThreadSafeQueue<Job*>*> _queues;
    static std::vector<std::condition_variable*> _signals;
    std::condition_variable* _main_job_signal = nullptr;
    JobConsumer* _generic_consumer = nullptr;
    JobConsumer* _io_consumer = nullptr;
    JobConsumer* _main_consumer = nullptr;
    int _requested_generic_count = 0;
    std::size_t _requested_category_count = 0;
    bool _is_running = false;
    std::mutex _generic_mutex{};
    friend class JobConsumer;
};