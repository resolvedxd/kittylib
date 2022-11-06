#include <algorithm>
#include <numbers>
// winapi is retarded...
#undef max
#undef min

namespace kmath {
    template <typename T>
    inline T clamp(T value, T min, T max) {
        return std::max(min, std::min(value, max));
    }

    template <typename T>
    inline T lerp(T a, T b, float t) {
        return a + (b - a) * t;
    }

    template <typename T>
    inline T map(T value, T min, T max, T new_min, T new_max) {
        return lerp(new_min, new_max, (value - min) / (max - min));
    }

    inline float radians(float degrees) {
        return degrees * (std::numbers::pi_v<float> / 180.0f);
    }

    inline float degrees(float radians) {
        return radians * (180.0f / std::numbers::pi_v<float>);
    }

    template<typename T>
    static constexpr inline T pown(T x, unsigned p) {
        T result = 1;

        while (p) {
            if (p & 0x1) {
                result *= x;
            }
            x *= x;
            p >>= 1;
        }

        return result;
    }

    // TODO: implement this with fast bezier (midpoint quadratic bezier)
    template <typename T>
    static constexpr inline T bezier(T p0, T p1, T p2, T p3, float t) {
      return pown<T>(1 - t, 3) * p0 + 3 * t * pown<T>(1 - t, 2) * p1 + 3 * pown<T>(t, 2) * (1 - t) * p2 + pown<T>(t, 3) * p3;
    }
    // float bezier(float p0, float p1, float p2, float p3, float t) {
        // return pow(1 - t, 3) * p0 + 3 * t * pow(1 - t, 2) * p1 + 3 * pow(t, 2) * (1 - t) * p2 + pow(t, 3) * p3;
    // }
}