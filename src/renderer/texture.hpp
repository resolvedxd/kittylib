#ifndef KITTY_TEXTURE
#define KITTY_TEXTURE
#include "../utils/vec2.hpp"

struct access_info_t {
  int width;
  int height;
  int pitch;
  void* data;
};

enum texture_format_t {
  FORMAT_ARGB, // A8 R8 G8 B8
  FORMAT_ALPHA, // A8
};

class texture_t {
  public:
  int width;
  int height;
  bool initialized;
  texture_format_t format;
  void* data;

  virtual access_info_t access(int x1, int y1, int width, int height) = 0;
  virtual void commit() = 0;
  virtual void destroy() = 0;

  inline access_info_t access() {
    return access(0, 0, width, height);
  }
  inline access_info_t access(vec2_t pos1, vec2_t pos2) {
    return access(pos1.x, pos1.y, pos2.x - pos1.x, pos2.y - pos1.y);
  }
};
#endif