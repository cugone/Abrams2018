#include "Engine/Math/MathUtils.hpp"

#include <cmath>

namespace MathUtils {

namespace {
unsigned int MT_RANDOM_SEED = 0;
}

void SetRandomEngineSeed(unsigned int seed) {
    MT_RANDOM_SEED = seed;
}

float ConvertDegreesToRadians(float degrees) {
    return degrees * (MathUtils::M_PI / 180.0f);
}

float ConvertRadiansToDegrees(float radians) {
    return radians * (180.0f * MathUtils::M_1_PI);
}

//NOT THREAD SAFE
std::random_device& GetRandomDevice() {
    static std::random_device rd;
    return rd;
}

//NOT THREAD SAFE
std::mt19937& GetMTRandomEngine(unsigned int seed /*= 0*/) {
    static std::mt19937 e = std::mt19937(!seed ? GetRandomDevice()() : seed);
    return e;
}

//NOT THREAD SAFE
std::mt19937_64& GetMT64RandomEngine(unsigned int seed /*= 0*/) {
    static std::mt19937_64 e = std::mt19937_64(!seed ? GetRandomDevice()() : seed);
    return e;
}

int GetRandomIntLessThan(int maxValueNotInclusive) {
    std::uniform_int_distribution<int> d(0, maxValueNotInclusive - 1);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

int GetRandomIntInRange(int minInclusive, int maxInclusive) {
    std::uniform_int_distribution<int> d(minInclusive, maxInclusive);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));

}

long GetRandomLongLessThan(long maxValueNotInclusive) {
    std::uniform_int_distribution<long> d(0L, maxValueNotInclusive - 1L);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

long GetRandomLongInRange(long minInclusive, long maxInclusive) {
    std::uniform_int_distribution<long> d(minInclusive, maxInclusive);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));

}

float GetRandomFloatInRange(float minInclusive, float maxInclusive) {
    std::uniform_real_distribution<float> d(minInclusive, std::nextafter(maxInclusive, maxInclusive + 1.0f));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

float GetRandomFloatZeroToOne() {
    std::uniform_real_distribution<float> d(0.0f, std::nextafter(1.0f, 2.0f));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

float GetRandomFloatZeroUpToOne() {
    std::uniform_real_distribution<float> d(0.0f, 1.0f);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

float GetRandomFloatNegOneToOne() {
    return GetRandomFloatInRange(-1.0f, 1.0f);
}

double GetRandomDoubleInRange(double minInclusive, double maxInclusive) {
    std::uniform_real_distribution<double> d(minInclusive, std::nextafter(maxInclusive, maxInclusive + 1.0));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

double GetRandomDoubleZeroToOne() {
    std::uniform_real_distribution<double> d(0.0, std::nextafter(1.0, 2.0));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

double GetRandomDoubleZeroUpToOne() {
    std::uniform_real_distribution<double> d(0.0, 1.0);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

double GetRandomDoubleNegOneToOne() {
    return GetRandomDoubleInRange(-1.0, 1.0);
}

long double GetRandomLongDoubleInRange(long double minInclusive, long double maxInclusive) {
    std::uniform_real_distribution<long double> d(minInclusive, std::nextafter(maxInclusive, maxInclusive + 1.0L));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

long double GetRandomLongDoubleZeroToOne() {
    std::uniform_real_distribution<long double> d(0.0L, std::nextafter(1.0L, 2.0L));
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

long double GetRandomLongDoubleZeroUpToOne() {
    std::uniform_real_distribution<long double> d(0.0L, 1.0L);
    return d(GetMTRandomEngine(MT_RANDOM_SEED));
}

long double GetRandomLongDoubleNegOneToOne() {
    return GetRandomDoubleInRange(-1.0L, 1.0L);
}

bool IsPercentChance(float probability) {
    float roll = GetRandomFloatZeroToOne();
    return roll < probability;
}

bool IsPercentChance(double probability) {
    double roll = GetRandomDoubleZeroToOne();
    return roll < probability;
}

bool IsPercentChance(long double probability) {
    long double roll = GetRandomLongDoubleZeroToOne();
    return roll < probability;
}

float CosDegrees(float degrees) {
    float radians = MathUtils::ConvertDegreesToRadians(degrees);
    return std::cos(radians);
}

float SinDegrees(float degrees) {
    float radians = MathUtils::ConvertDegreesToRadians(degrees);
    return std::sin(radians);
}

float Atan2Degrees(float y, float x) {
    float radians = std::atan2(y, x);
    return MathUtils::ConvertRadiansToDegrees(radians);
}

bool IsEquivalent(float a, float b, float epsilon /*= 0.00001f*/) {
    return std::abs(a - b) < epsilon;
}

bool IsEquivalent(double a, double b, double epsilon /*= 0.0001*/) {
    return std::abs(a - b) < epsilon;
}

bool IsEquivalent(long double a, long double b, long double epsilon /*= 0.0001L*/) {
    return std::abs(a - b) < epsilon;
}

bool IsEquivalent(const Vector2& a, const Vector2& b, float epsilon /*= 0.0001f*/) {
    return IsEquivalent(a.x, b.x, epsilon) && IsEquivalent(a.y, b.y, epsilon);
}

bool IsEquivalent(const Vector3& a, const Vector3& b, float epsilon /*= 0.0001f*/) {
    return IsEquivalent(a.x, b.x, epsilon) && IsEquivalent(a.y, b.y, epsilon) && IsEquivalent(a.z, b.z, epsilon);
}

float DotProduct(const Vector2& a, const Vector2& b) {
    return a.x * b.x + a.y * b.y;
}

float DotProduct(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 CrossProduct(const Vector3& a, const Vector3& b) {
    float a1 = a.x;
    float a2 = a.y;
    float a3 = a.z;

    float b1 = b.x;
    float b2 = b.y;
    float b3 = b.z;

    return Vector3(a2 * b3 - a3 * b2, a3 * b1 - a1 * b3, a1 * b2 - a2 * b1);
}

Vector2 Project(const Vector2& a, const Vector2& b) {
    return (DotProduct(a, b) / DotProduct(b, b)) * b;
}

Vector3 Project(const Vector3& a, const Vector3& b) {
    return (DotProduct(a, b) / DotProduct(b, b)) * b;
}

Vector2 Reflect(const Vector2& in, const Vector2& normal) {
    return in - ((2.0f * DotProduct(in, normal)) * normal);
}

Vector3 Reflect(const Vector3& in, const Vector3& normal) {
    return in - ((2.0f * DotProduct(in, normal)) * normal);
}

Vector2 ProjectAlongPlane(const Vector2& v, const Vector2& n) {
    return v - (DotProduct(v, n) * n);
}

Vector3 ProjectAlongPlane(const Vector3& v, const Vector3& n) {
    return v - (DotProduct(v, n) * n);
}

} //End MathUtils