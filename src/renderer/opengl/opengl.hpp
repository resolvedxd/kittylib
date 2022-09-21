#pragma once
#ifdef _WIN32
#include <Windows.h>
#endif
#include <gl/GL.h>
#include <gl/GLU.h>
#include <vector>
#include "../../utils/color.hpp"
#include "../renderer.hpp"
#include "../../utils/renderlist.hpp"

class opengl_renderer_t : public renderer_t 
{
  public:
  opengl_renderer_t();

  HGLRC hrc;
  render_list_t* render_list;

  virtual bool initialize() { return false; };
  virtual bool uninitialize();

  virtual void begin();
  virtual void end();

  virtual void line(float x1, float y1, float x2, float y2, color_t color);
  virtual void line(float x1, float y1, float x2, float y2, float width, color_t color) {};
  virtual void rect(float x, float y, float width, float height, color_t color) {};
  virtual void rect_filled(float x, float y, float width, float height, color_t color) {};
  virtual void rect_gradient(float x, float y, float width, float height, color_t top_left, color_t top_right, 
    color_t bottom_left, color_t bottom_right) {};
  virtual void rect_gradient(float x, float y, float width, float height, color_t left_top, color_t right_bottom,
    bool horizontal = true) {};
  virtual void circle(float x, float y, float radius, color_t color, int segments) {};
  virtual void circle_filled(float x, float y, float radius, color_t color, int segments) {};
  virtual void triangle(float x, float y, float w, float h, float angle, color_t color) {};
  virtual void triangle_filled(float x, float y, float w, float h, float angle, color_t color) {};
}; // class renderer_t
