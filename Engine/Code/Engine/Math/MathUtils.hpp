#pragma once

#include <algorithm>
#include <random>
#include <utility>

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/IntVector4.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

class AABB2;

namespace MathUtils {

constexpr const float M_1PI_6               = 0.52359877559829887307f;            // 1pi/6
constexpr const float M_1PI_4               = 0.78539816339744830962f;            // 1pi/4
constexpr const float M_1PI_3               = 1.04719755119659774615f;            // 1pi/3
constexpr const float M_1PI_2               = 1.57079632679489661923f;            // 1pi/2
constexpr const float M_2PI_3               = 2.09439510239319549230f;            // 2pi/3
constexpr const float M_3PI_4               = 2.35619449019234492884f;            // 3pi/4
constexpr const float M_5PI_6               = 2.61799387799149436538f;            // 5pi/6
constexpr const float M_PI                  = 3.14159265358979323846f;            // pi
constexpr const float M_7PI_6               = 3.66519142918809211153f;            // 7pi/6
constexpr const float M_5PI_4               = 3.92699081698724154807f;            // 5pi/4
constexpr const float M_4PI_3               = 4.18879020478639098461f;            // 4pi/3
constexpr const float M_3PI_2               = 4.71238898038468985769f;            // 3pi/2
constexpr const float M_5PI_3               = 5.23598775598298873077f;            // 5pi/3
constexpr const float M_7PI_4               = 5.49778714378213816730f;            // 7pi/4
constexpr const float M_11PI_6              = 5.75958653158128760384f;            // 11pi/6
constexpr const float M_2PI                 = 6.28318530717958647692f;            // 2pi
constexpr const float M_E                   = 2.71828182845904523536f;            // e
constexpr const float M_LOG2E               = 1.44269504088896340736f;            // log2(e)
constexpr const float M_LOG10E              = 0.43429448190325182765f;            // log10(e)
constexpr const float M_LN2                 = 0.69314718055994530942f;            // ln(2)
constexpr const float M_LN10                = 2.30258509299404568402f;            // ln(10)
constexpr const float M_PI_2                = 1.57079632679489661923f;            // pi/2
constexpr const float M_PI_4                = 0.78539816339744830962f;            // pi/4
constexpr const float M_1_PI                = 0.31830988618379067151f;            // 1/pi
constexpr const float M_2_PI                = 0.63661977236758134308f;            // 2/pi
constexpr const float M_2_SQRTPI            = 1.12837916709551257390f;            // 2/sqrt(pi)
constexpr const float M_SQRT2               = 1.41421356237309504880f;            // sqrt(2)
constexpr const float M_1_SQRT2             = 0.70710678118654752440f;            // 1/sqrt(2)
constexpr const float M_SQRT3_3             = 0.57735026918962576451f;            // sqrt(3)/3
constexpr const float M_TAU                 = 1.61803398874989484821f;            // tau (golden ratio)
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
bool IsEquivalent(const Vector4& a, const Vector4& b, float epsilon = 0.0001f);

float CalcDistance(const Vector2& a, const Vector2& b);
float CalcDistance(const Vector3& a, const Vector3& b);
float CalcDistance(const Vector4& a, const Vector4& b);

float CalcDistanceSquared(const Vector2& a, const Vector2& b);
float CalcDistanceSquared(const Vector3& a, const Vector3& b);
float CalcDistanceSquared(const Vector4& a, const Vector4& b);

Vector3 CrossProduct(const Vector3& a, const Vector3& b);

float DotProduct(const Vector2& a, const Vector2& b);
float DotProduct(const Vector3& a, const Vector3& b);
float DotProduct(const Vector4& a, const Vector4& b);

Vector2 Project(const Vector2& a, const Vector2& b);
Vector3 Project(const Vector3& a, const Vector3& b);
Vector4 Project(const Vector4& a, const Vector4& b);

Vector2 Reflect(const Vector2& in, const Vector2& normal);
Vector3 Reflect(const Vector3& in, const Vector3& normal);
Vector4 Reflect(const Vector4& in, const Vector4& normal);

Vector2 ProjectAlongPlane(const Vector2& v, const Vector2& n);
Vector3 ProjectAlongPlane(const Vector3& v, const Vector3& n);
Vector4 ProjectAlongPlane(const Vector4& v, const Vector4& n);

unsigned int CalculateManhattanDistance(const IntVector2& start, const IntVector2& end);
unsigned int CalculateManhattanDistance(const IntVector3& start, const IntVector3& end);
unsigned int CalculateManhattanDistance(const IntVector4& start, const IntVector4& end);

bool IsPointInside(const AABB2& aabb, const Vector2& point);
Vector2 CalcClosestPoint(const Vector2& p, const AABB2& aabb);

bool DoAABBsOverlap(const AABB2& a, const AABB2& b);


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

template<>
Vector2 Clamp<Vector2>(const Vector2& valueToClamp, const Vector2& minRange, const Vector2& maxRange);

template<>
Vector3 Clamp<Vector3>(const Vector3& valueToClamp, const Vector3& minRange, const Vector3& maxRange);

template<>
Vector4 Clamp<Vector4>(const Vector4& valueToClamp, const Vector4& minRange, const Vector4& maxRange);

template<>
IntVector2 Clamp<IntVector2>(const IntVector2& valueToClamp, const IntVector2& minRange, const IntVector2& maxRange);

template<>
IntVector3 Clamp<IntVector3>(const IntVector3& valueToClamp, const IntVector3& minRange, const IntVector3& maxRange);

template<>
IntVector4 Clamp<IntVector4>(const IntVector4& valueToClamp, const IntVector4& minRange, const IntVector4& maxRange);

template<typename T>
T Interpolate(const T& a, const T& b, float t) {
    return ((1.0f - t) * a) + (t * b);
}

template<>
Vector2 Interpolate(const Vector2& a, const Vector2& b, float t);

template<>
Vector3 Interpolate(const Vector3& a, const Vector3& b, float t);

template<>
Vector4 Interpolate(const Vector4& a, const Vector4& b, float t);

template<>
IntVector2 Interpolate(const IntVector2& a, const IntVector2& b, float t);

template<>
IntVector3 Interpolate(const IntVector3& a, const IntVector3& b, float t);

template<>
IntVector4 Interpolate(const IntVector4& a, const IntVector4& b, float t);

template<typename T>
T RangeMap(const T& valueToMap, const T& minInputRange, const T& maxInputRange, const T& minOutputRange, const T& maxOutputRange) {
    return (valueToMap - minInputRange) * (maxOutputRange - minOutputRange) / (maxInputRange - minInputRange) + minOutputRange;
}

namespace EasingFunctions {

template<typename T, std::size_t... Is>
T SmoothStart_helper(const T& t, std::index_sequence<Is...>) {
    return (((void)Is, t) * ...);
}

template<std::size_t N, typename T>
T SmoothStart(const T& t) {
    static_assert(std::is_floating_point_v<T>, "SmoothStart requires T to be non-integral.");
    static_assert(N > 0, "SmoothStart requires value of  to be non-negative and non-zero.");
    return SmoothStart_helper(t, std::make_index_sequence<N>{});
}


template<typename T, std::size_t... Is>
T SmoothStop_helper(const T& t, std::index_sequence<Is...>) {
    return (((void)Is, (1.0f - t)) * ...);
}

template<std::size_t N, typename T>
T SmoothStop(const T& t) {
    static_assert(std::is_floating_point_v<T>, "SmoothStop requires T to be non-integral.");
    static_assert(N > 0, "SmoothStop requires value of  to be non-negative and non-zero.");
    return SmoothStop_helper(t, std::make_index_sequence<N>{});
}

template<std::size_t N, typename T>
T SmoothStep(const T& t) {
    static_assert(std::is_floating_point_v<T>, "SmoothStop requires T to be non-integral.");
    static_assert(N > 0, "SmoothStop requires value of  to be non-negative and non-zero.");
    return Interpolate(SmoothStart<N>(t), SmoothStop<N>(t), 0.5f);
}


} //End EasingFunctions

} //End MathUtils