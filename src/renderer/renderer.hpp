#ifndef KITTY_RENDERER
#define KITTY_RENDERER
#include "../utils/color.hpp"
#include "../utils/vec2.hpp"
#include "texture.hpp"

struct kvertex_t {
  float x, y, z;
  float rhw = 0;
  color_t color;
};

class renderer_t
{
  public:
  bool initialized;
  bool antialias = true;
  bool alpha_blending = true;
  virtual bool initialize() { return false; };
  virtual bool uninitialize() { return false; };

  virtual void begin() = 0;
  virtual void end() = 0;

  virtual void line(float x1, float y1, float x2, float y2, color_t color) = 0;
  virtual void line(float x1, float y1, float x2, float y2, float width, color_t color) = 0;
  virtual void rect(float x, float y, float width, float height, color_t color) = 0;
  virtual void rect_filled(float x, float y, float width, float height, color_t color) = 0;
  virtual void rect_gradient(float x, float y, float width, float height, color_t top_left, color_t top_right, 
    color_t bottom_left, color_t bottom_right) = 0;
  virtual void rect_gradient(float x, float y, float width, float height, color_t left_top, color_t right_bottom,
    bool horizontal = true) = 0;
  virtual void rect_textured(float x, float y, texture_t* texture, color_t color = color_t(255, 255, 255)) = 0;
  // virtual void rect_rounded(float x, float y, float width, float height, float radius, color_t color, int segments) = 0;
  virtual void circle(float x, float y, float radius, color_t color, int segments) = 0;
  virtual void circle_filled(float x, float y, float radius, color_t color, int segments) = 0;
  virtual void triangle(float x, float y, float w, float h, float angle, color_t color) = 0;
  virtual void triangle_filled(float x, float y, float w, float h, float angle, color_t color) = 0;
  virtual void vertices(kvertex_t* vertices, int count, primitive_type_t type) = 0;

  // textures
  virtual texture_t* create_texture(int width, int height, texture_format_t format = FORMAT_ARGB) = 0;

  // other
  virtual void set_viewport(int x, int y, int width, int height) = 0;

  // Vector helpers
  inline void line(vec2_t p1, vec2_t p2, color_t color) {
    line(p1.x, p1.y, p2.x, p2.y, color);
  }
  inline void line(vec2_t p1, vec2_t p2, float width, color_t color) {
    line(p1.x, p1.y, p2.x, p2.y, width, color);
  }
  inline void rect(vec2_t p, vec2_t size, color_t color) {
    rect(p.x, p.y, size.x, size.y, color);
  }
  inline void rect_filled(vec2_t p, vec2_t size, color_t color) {
    rect_filled(p.x, p.y, size.x, size.y, color);
  }
  inline void rect_gradient(vec2_t p, vec2_t size, color_t top_left, color_t top_right, color_t bottom_left, color_t bottom_right) {
    rect_gradient(p.x, p.y, size.x, size.y, top_left, top_right, bottom_left, bottom_right);
  }
  inline void rect_gradient(vec2_t p, vec2_t size, color_t left_top, color_t right_bottom, bool horizontal = true) {
    rect_gradient(p.x, p.y, size.x, size.y, left_top, right_bottom, horizontal);
  }
  inline void rect_textured(vec2_t p, texture_t* texture, color_t color = color_t(255, 255, 255)) {
    rect_textured(p.x, p.y, texture, color);
  }
  inline void circle(vec2_t p, float radius, color_t color, int segments) {
    circle(p.x, p.y, radius, color, segments);
  }
  inline void circle_filled(vec2_t p, float radius, color_t color, int segments) {
    circle_filled(p.x, p.y, radius, color, segments);
  }
  inline void triangle(vec2_t p, vec2_t size, float angle, color_t color) {
    triangle(p.x, p.y, size.x, size.y, angle, color);
  }
  inline void triangle_filled(vec2_t p, vec2_t size, float angle, color_t color) {
    triangle_filled(p.x, p.y, size.x, size.y, angle, color);
  }
};
#endif
