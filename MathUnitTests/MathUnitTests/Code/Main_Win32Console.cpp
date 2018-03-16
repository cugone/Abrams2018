
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <chrono>

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

#include "Engine/Core/TimeUtils.hpp"

struct TestResults {
    unsigned int total_tests = 0;
    unsigned int passed_tests = 0;
    unsigned int failed_tests = 0;
};

TestResults results;

void ApplyTest(std::string_view test_string, const std::function<bool()>& test);
int OutputResults();

#pragma region Tests
void TestVector2();
void TestVector3();
void TestMathUtils();
void TestSplit();
void TestJoin();
#pragma endregion

int main(int /*argc*/, char** /*argv*/) {
    
    TestVector2();
    TestVector3();
    TestMathUtils();
    TestSplit();
    TestJoin();
    unsigned int failed_tests = OutputResults();
    return failed_tests;
}

void ApplyTest(std::string_view test_string, const std::function<bool()>& test) {
    std::size_t max_test_string_length = 105;
    test_string.remove_suffix(test_string.size() - (std::min)(test_string.size(), max_test_string_length));
    constexpr int test_string_field_width = 107;
    std::cout << '\n' << std::setw(test_string_field_width) << std::left << test_string;
    ++results.total_tests;
    if(test()) {
        ++results.passed_tests;
        constexpr int passed_field_width = 9;
        std::cout << std::setw(passed_field_width) << std::right << "PASSED";
    } else {
        ++results.failed_tests;
        constexpr int failed_field_width = 12;
        std::cout << std::setw(failed_field_width) << std::right << "!!!FAILED!!!";
    }
}

int OutputResults() {

    std::cout << "\n\nTOTAL TESTS:"   << std::setw(16) << std::setfill('.') << std::right << results.total_tests;
    std::cout << "\nPASSED TESTS:"    << std::setw(15) << std::setfill('.') << std::right << results.passed_tests;
    std::cout << "\nFAILED TESTS:"    << std::setw(15) << std::setfill('.') << std::right << results.failed_tests;
    double pass_ratio = static_cast<double>(results.passed_tests) / static_cast<double>(results.total_tests);
    double pass_perc = 100.0 * pass_ratio;
    std::cout << "\nPASS PERCENTAGE:" << std::setw(15) << std::setfill('.') << std::right << std::setprecision(2) << std::fixed << pass_perc << '%';

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

void TestSplit() {
    ApplyTest("Split returns one value on \"abc\"",
              []()->bool {
        auto result = StringUtils::Split("abc");
        return result.size() == 1
            && result[0] == "abc";
    });
    ApplyTest("Split returns zero values on \"\"",
              []()->bool {
        auto result = StringUtils::Split("");
        return result.empty();
    });
    ApplyTest("Split returns three values on \"a,b,c\"",
              []()->bool {
        auto result = StringUtils::Split("a,b,c");
        return result.size() == 3
            && result[0] == "a"
            && result[1] == "b"
            && result[2] == "c"
            ;
    });
    ApplyTest("Split returns two values by default on \"a,,c\"",
              []()->bool {
        auto result = StringUtils::Split("a,,c");
        return result.size() == 2
            && result[0] == "a"
            && result[1] == "c"
            ;
    });
    ApplyTest("Split returns three values with skip_empty OFF on \"a,,c\"",
              []()->bool {
        auto result = StringUtils::Split("a,,c", ',', false);
        return result.size() == 3
            && result[0] == "a"
            && result[1] == ""
            && result[2] == "c"
            ;
    });
}

void TestJoin() {
    ApplyTest("Join returns \"abc\" from \"{{a},{b},{c}}\"",
              []()->bool {
        auto result = StringUtils::Join({ { "a" },{ "b" },{ "c" } });
        return result == "abc";
    });
    ApplyTest("Join returns \"a,b,c\" from \"{{a},{b},{c}}\" with delim ',':",
              []()->bool {
        auto result = StringUtils::Join({ { "a" },{ "b" },{ "c" } }, ',');
        return result == "a,b,c";
    });
}
