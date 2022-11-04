#pragma once
#include <Windows.h> // d3d9 is windows only anyway..
#include <d3d9.h>
#include "../renderer.hpp"
#include "../../utils/renderlist.hpp"
#include "../d3d9/d3d9_tex_impl.hpp"
#include "../../os/os.hpp"

inline D3DCOLOR to_d3d(color_t color) {
  return D3DCOLOR_ARGB(color.a, color.r, color.g, color.b);
};

class d3d9_renderer_t : public renderer_t {
  public:
  d3d9_renderer_t();
  bool initialize(IDirect3DDevice9* device);

  IDirect3DDevice9* device;
  render_list_t* render_list;
  D3DVIEWPORT9 viewport = { 0, 0, 100, 100, 0.0f, 1.0f };
  D3DPRESENT_PARAMETERS d3dpp;

  std::vector<d3d9_texture_t*> textures;

  virtual void begin();
  virtual void end();

  virtual void line(float x1, float y1, float x2, float y2, color_t color);
  virtual void line(float x1, float y1, float x2, float y2, float width, color_t color);
  virtual void rect(float x, float y, float width, float height, color_t color);
  virtual void rect_filled(float x, float y, float width, float height, color_t color);
  virtual void rect_gradient(float x, float y, float width, float height, color_t top_left, color_t top_right, 
    color_t bottom_left, color_t bottom_right);
  virtual void rect_gradient(float x, float y, float width, float height, color_t left_top, color_t right_bottom,
    bool horizontal = true);
  virtual void rect_textured(float x, float y, texture_t* texture, color_t color = color_t(255, 255, 255));
  virtual void rect_rounded(float x, float y, float width, float height, float radius, color_t color, int segments);
  virtual void circle(float x, float y, float radius, color_t color, int segments);
  virtual void circle_filled(float x, float y, float radius, color_t color, int segments);
  virtual void triangle(float x, float y, float w, float h, float angle, color_t color) {};
  virtual void triangle_filled(float x, float y, float w, float h, float angle, color_t color) {};
  virtual void vertices(kvertex_t* vertices, int count, primitive_type_t type) {};

  virtual texture_t* create_texture(int width, int height, texture_format_t format = FORMAT_ARGB);
  // begin rendering to a texture
  virtual void begin_texture(int width, int height, texture_format_t format = FORMAT_ARGB);
  virtual void begin_texture(texture_t* texture);
  // end rendering to a texture
  virtual texture_t* end_texture();

  virtual void set_viewport(int x, int y, int width, int height);

  private:
  using renderer_t::initialize; // hide the original initialize function
}; // class d3d9_renderer_t