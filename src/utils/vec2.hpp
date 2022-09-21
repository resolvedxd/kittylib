#ifndef KITTY_VEC2_T
#define KITTY_VEC2_T
#include <math.h>

class vec2_t {
public:
  float x, y;

  vec2_t() {
    x = 0; y = 0;
  };
  vec2_t(float _x, float _y) {
    x = _x; y = _y;
  };
  // vec2_t(vec3_t vec) {
  //   x = vec.x; y = vec.y;
  // }

  inline vec2_t operator*(const float n) const {
    return vec2_t(x * n, y * n);
  }
  inline vec2_t operator+(const vec2_t& v) const {
    return vec2_t(x + v.x, y + v.y);
  }
  inline vec2_t operator-(const vec2_t& v) const {
    return vec2_t(x - v.x, y - v.y);
  }
  inline void operator+=(const vec2_t& v) {
    x += v.x;
    y += v.y;
  }
  inline void operator-=(const vec2_t& v) {
    x -= v.x;
    y -= v.y;
  }

  bool operator==(const vec2_t& v) const {
    return (v.x == x && v.y == y);
  }
  bool operator!=(const vec2_t& v) const {
    return (v.x != x || v.y != y);
  }

  inline float length() {
    return sqrt((x * x) + (y * y));
  }

  inline float dist_to(vec2_t v) {
    return sqrt(pow(v.x - x, 2) + pow(v.y - y, 2));
  }

  inline vec2_t lerp(vec2_t v, float t) {
    return vec2_t(x + (v.x - x) * t, y + (v.y - y) * t);
  }

  inline vec2_t normalize() {
    float len = length();
    return vec2_t(x / len, y / len);
  }

  inline vec2_t rotate(vec2_t p, float angle) {
    return vec2_t(cos(angle) * (p.x - x) - sin(angle) * (p.y - y) + x,
      sin(angle) * (p.x - x) + cos(angle) * (p.y - y) + y);
  }

  // inline void print() {
  //   #ifdef _DEBUG
  //   printf("(%f, %f)\n", x, y);
  //   #endif
  // }
};

#endif // VEC2_T_HPP