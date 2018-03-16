
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>

#include <thread>
#include <chrono>

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

#include "Engine/Core/Time.hpp"

struct TestResults {
    unsigned int total_tests = 0;
    unsigned int passed_tests = 0;
    unsigned int failed_tests = 0;
};

TestResults results;

void ApplyTest(const std::string& test_string, const std::function<bool()>& test);
int OutputResults();

#pragma region Tests
void TestVector2();
void TestVector3();
void TestMathUtils();
#pragma endregion

int main(int /*argc*/, char** /*argv*/) {
    
    TestVector2();
    TestVector3();
    TestMathUtils();
    unsigned int failed_tests = OutputResults();
    return failed_tests;
}

void ApplyTest(const std::string& test_string, const std::function<bool()>& test) {
    std::cout << '\n' << std::setw(110) << std::left << test_string;
    ++results.total_tests;
    if(test()) {
        ++results.passed_tests;
        std::cout << "PASSED";
    } else {
        ++results.failed_tests;
        std::cout << "!!!FAILED!!!";
    }
}

int OutputResults() {

    std::cout << "\n\nTOTAL TESTS:"   << std::setw(16) << std::setfill('.') << std::right << results.total_tests;
    std::cout << "\nPASSED TESTS:"    << std::setw(15) << std::setfill('.') << std::right << results.passed_tests;
    std::cout << "\nFAILED TESTS:"    << std::setw(15) << std::setfill('.') << std::right << results.failed_tests;
    std::cout << "\nPASS PERCENTAGE:" << std::setw(12) << std::setfill('.') << std::right << std::setprecision(2) << std::fixed << static_cast<double>(results.passed_tests) / static_cast<double>(results.total_tests);

    return results.failed_tests;
}

void TestVector2() {

    ApplyTest("RangeMap of Vec2(127,127) to Vec2(0-1,0-1) produces Vec2(0.5,0.5):",
    [&]()->bool{
        Vector2 expected(0.5f, 0.5f);
        Vector2 test(127.0f, 127.0f);
        Vector2 actual = MathUtils::RangeMap(test, Vector2::ZERO, Vector2::ONE * 255.0f, Vector2::ZERO, Vector2::ONE);
        return MathUtils::IsEquivalent(actual, expected, 0.002f);
    });

}

void TestVector3() {

}

void TestMathUtils() {

    ApplyTest("Cross X and Y == Z:",
    []()->bool{
        Vector3 expected = Vector3::Z_AXIS;
        Vector3 actual = MathUtils::CrossProduct(Vector3::X_AXIS, Vector3::Y_AXIS);
        return MathUtils::IsEquivalent(actual, expected);
    });

}
