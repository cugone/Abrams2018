#pragma once

#include <algorithm>
#include <random>
#include <utility>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

namespace MathUtils {

constexpr const float M_PI = 3.14159265358979323846f;                             // pi
constexpr const float M_E = 2.71828182845904523536f;                              // e
constexpr const float M_LOG2E = 1.44269504088896340736f;                          // log2(e)
constexpr const float M_LOG10E = 0.43429448190325182765f;                         // log10(e)
constexpr const float M_LN2 = 0.69314718055994530942f;                            // ln(2)
constexpr const float M_LN10 = 2.30258509299404568402f;                           // ln(10)
constexpr const float M_PI_2 = 1.57079632679489661923f;                           // pi/2
constexpr const float M_PI_4 = 0.78539816339744830962f;                           // pi/4
constexpr const float M_1_PI = 0.31830988618379067151f;                           // 1/pi
constexpr const float M_2_PI = 0.63661977236758134308f;                           // 2/pi
constexpr const float M_2_SQRTPI = 1.12837916709551257390f;                       // 2/sqrt(pi)
constexpr const float M_SQRT2 = 1.41421356237309504880f;                          // sqrt(2)
constexpr const float M_1_SQRT2 = 0.70710678118654752440f;                        // 1/sqrt(2)
constexpr const float M_SQRT3_3 = 0.57735026918962576451f;                        // sqrt(3)/3
constexpr const float M_TAU = 1.61803398874989484821f;                            // tau (golden ratio)
constexpr const long double KIB_BYTES_RATIO = 0.0009765625;                       // Kilobyte/Bytes
constexpr const long double MIB_BYTES_RATIO = 0.00000095367431640625;             // Megabyte/Bytes
constexpr const long double GIB_BYTES_RATIO = 0.000000000931322574615478515625;   // Gigabyte/Bytes
constexpr const long double BYTES_KIB_RATIO = 1024.0;                             // Bytes/Kilobytes
constexpr const long double BYTES_MIB_RATIO = 1048576.0;                          // Bytes/Megabytes
constexpr const long double BYTES_GIB_RATIO = 1073741824.0;                       // Bytes/Gigabytes

//NOT THREAD SAFE
void SetRandomEngineSeed(unsigned int seed);
//NOT THREAD SAFE
std::random_device& GetRandomDevice();
//NOT THREAD SAFE
std::mt19937& GetMTRandomEngine(unsigned int seed = 0);
//NOT THREAD SAFE
std::mt19937_64& GetMT64RandomEngine(unsigned int seed = 0);

float ConvertDegreesToRadians(float degrees);
float ConvertRadiansToDegrees(float radians);

int GetRandomIntLessThan(int maxValueNotInclusive);
int GetRandomIntInRange(int minInclusive, int maxInclusive);

long GetRandomLongLessThan(long maxValueNotInclusive);
long GetRandomLongInRange(long minInclusive, long maxInclusive);

float GetRandomFloatInRange(float minInclusive, float maxInclusive);
float GetRandomFloatZeroToOne();
float GetRandomFloatZeroUpToOne();
float GetRandomFloatNegOneToOne();
bool IsPercentChance(float probability);

double GetRandomDoubleInRange(double minInclusive, double maxInclusive);
double GetRandomDoubleZeroToOne();
double GetRandomDoubleZeroUpToOne();
double GetRandomDoubleNegOneToOne();
bool IsPercentChance(double probability);

long double GetRandomLongDoubleInRange(long double minInclusive, long double maxInclusive);
long double GetRandomLongDoubleZeroToOne();
long double GetRandomLongDoubleZeroUpToOne();
long double GetRandomLongDoubleNegOneToOne();
bool IsPercentChance(long double probability);


float CosDegrees(float degrees);
float SinDegrees(float degrees);
float Atan2Degrees(float y, float x);

bool IsEquivalent(float a, float b, float epsilon = 0.00001f);
bool IsEquivalent(double a, double b, double epsilon = 0.0001);
bool IsEquivalent(long double a, long double b, long double epsilon = 0.0001L);
bool IsEquivalent(const Vector2& a, const Vector2& b, float epsilon = 0.0001f);
bool IsEquivalent(const Vector3& a, const Vector3& b, float epsilon = 0.0001f);

Vector3 CrossProduct(const Vector3& a, const Vector3& b);

float DotProduct(const Vector2& a, const Vector2& b);
float DotProduct(const Vector3& a, const Vector3& b);

Vector2 Project(const Vector2& a, const Vector2& b);
Vector3 Project(const Vector3& a, const Vector3& b);

Vector2 Reflect(const Vector2& in, const Vector2& normal);
Vector3 Reflect(const Vector3& in, const Vector3& normal);


template<typename T>
T Clamp(const T& valueToClamp, const T& minRange, const T& maxRange) {
    if(valueToClamp < minRange) {
        return minRange;
    }
    if(maxRange < valueToClamp) {
        return maxRange;
    }
    return valueToClamp;
}


}