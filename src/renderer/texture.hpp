#ifndef KITTY_TEXTURE
#define KITTY_TEXTURE
#include "../utils/vec2.hpp"

enum primitive_type_t {
  PT_POINTLIST             = 1,
  PT_LINELIST              = 2,
  PT_LINESTRIP             = 3,
  PT_TRIANGLELIST          = 4,
  PT_TRIANGLESTRIP         = 5,
  PT_TRIANGLEFAN           = 6,
};

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
  void* data = 0;

  virtual access_info_t access(int x1, int y1, int width, int height) = 0;
  virtual void commit() = 0;
  virtual void destroy() = 0;

  inline access_info_t access() {
    return access(0, 0, width, height);
  }
  inline access_info_t access(vec2_t pos1, vec2_t pos2) {
    return access(pos1.x, pos1.y, pos2.x - pos1.x, pos2.y - pos1.y);
  }

  inline bool alpha_mask(texture_t* mask) {
    // if null is passed, clear the alpha mask
    if (mask == nullptr) {
      alpha_masked = false;
      mask = nullptr;
      return true;
    }

    if (mask->format != FORMAT_ALPHA) return false;
    if (mask->width != width || mask->height != height) return false;
    
    alpha_masked = true;
    this->mask = mask;
    return true;
  }

  bool alpha_masked = false; // wheter this texture is alpha masked
  texture_t* mask = nullptr; // the alpha mask texture
};
#endif