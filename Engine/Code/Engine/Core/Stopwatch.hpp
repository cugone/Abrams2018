#pragma once

#include "Engine/Core/TimeUtils.hpp"

class Stopwatch {
public:
    void SetSeconds(float seconds);
    void SetFrequency(float hz);
    bool Check();
    bool CheckAndDecrement();
    bool CheckAndReset();
    unsigned int DecrementAll();
    void Reset();
private:
    FPSeconds interval_time;
    FPSeconds target_time;

};