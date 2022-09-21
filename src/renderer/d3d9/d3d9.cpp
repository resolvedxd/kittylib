#include <cstdio>
#include <cstring>
#include <numbers>
#include <math.h>
#include <vector>
#include <d3d9.h>
#include "d3d9.hpp"
#include "../../kitty.hpp"
#include "../../os/windows.hpp"

d3d9_renderer_t::d3d9_renderer_t() {
  LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);
  
  D3DPRESENT_PARAMETERS d3dpp;
  std::memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
  d3dpp.Windowed = true;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // discard old frames
  d3dpp.hDeviceWindow = kitty::win32::hwnd;
  d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

  d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, kitty::win32::hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, 
    &this->device);

  if (!this->initialize(this->device)) {
    printf("Failed to initialize d3d9 renderer\n");
    exit(1);
  }
}

bool d3d9_renderer_t::initialize(IDirect3DDevice9* device) {
  if (!device) return false;
  this->device = device;
  this->render_list = new render_list_t();
  return true;
}

void d3d9_renderer_t::begin() {
  device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

  // anti aliasing
  device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS , antialias);
  device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, antialias);
  // device->SetRenderState(D3DRS_CLIPPING, true);

  // alpha blending
  device->SetRenderState(D3DRS_ALPHABLENDENABLE, alpha_blending);
  if (alpha_blending) {
  device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_BLENDDIFFUSEALPHA);
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, true);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
  }

  device->BeginScene();
}

void d3d9_renderer_t::end() {
  device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

  for (auto renderable : render_list->renderables) {
    switch (renderable->renderable_type) {
      case PRIMITIVE:
      {
        primitive_t* primitive = (primitive_t*)renderable;
        // printf("primitive type: %d count: %d\n", primitive->primitive_type, primitive->primitive_count());
        device->DrawPrimitiveUP((D3DPRIMITIVETYPE)primitive->primitive_type, primitive->primitive_count(), 
          primitive->vertices.data(), sizeof(vertex_t));
        break;
      }
      case TEXTURE:
      {

        primitive_t* primitive = (primitive_t*)renderable;

        device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        device->SetTexture(0, ((d3d9_texture_t*)primitive->texture)->texture);

        if (primitive->texture->format == FORMAT_ALPHA)
          device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        
        device->DrawPrimitiveUP((D3DPRIMITIVETYPE)primitive->primitive_type, primitive->primitive_count(), 
          primitive->vertices_textured.data(), sizeof(textured_vertex_t));

        // if (primitive->texture->format == FORMAT_ALPHA)
        device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        
        device->SetTexture(0, nullptr);
        device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
        break;
      }
      default:
        printf("unhandled renderable type: %d\n", renderable->renderable_type);
        break;
    }
  }

  render_list->empty();
  // render_list->renderables.clear();

  device->EndScene();
  device->Present(NULL, NULL, NULL, NULL);
}

void d3d9_renderer_t::line(float x1, float y1, float x2, float y2, color_t color) {
  D3DCOLOR color_d3d = to_d3d(color);
  vertex_t v[] = {
    { x1, y1, 0.0f, 0.0f, color_d3d },
    { x2, y2, 0.0f, 0.0f, color_d3d }
  };

  render_list->add(new primitive_t(PT_LINELIST, v, 2));
};

// line with thickness
void d3d9_renderer_t::line(float x1, float y1, float x2, float y2, float width, color_t color) {
  D3DCOLOR color_d3d = to_d3d(color);
  float width_half = width / 2.0f;
  vertex_t v[] = {
    { x1 - width_half, y1 - width_half, 0.0f, 0.0f, color_d3d },
    { x1 + width_half, y1 + width_half, 0.0f, 0.0f, color_d3d },
    { x2 - width_half, y2 - width_half, 0.0f, 0.0f, color_d3d },
    { x2 + width_half, y2 + width_half, 0.0f, 0.0f, color_d3d }
  };

  render_list->add(new primitive_t(PT_TRIANGLESTRIP, v, 4));
};

void d3d9_renderer_t::rect(float x, float y, float width, float height, color_t color) {
  D3DCOLOR color_d3d = to_d3d(color);
  vertex_t v[] = {
    { x, y, 0.0f, 1.0f, color_d3d },
    { x + width, y, 0.0f, 1.0f, color_d3d },

    { x + width, y, 0.0f, 1.0f, color_d3d },
    { x + width, y + height, 0.0f, 1.0f, color_d3d },

    { x, y, 0.0f, 1.0f, color_d3d },
    { x, y + height, 0.0f, 1.0f, color_d3d },

    { x, y + height, 0.0f, 1.0f, color_d3d },
    { x + width + 1, y + height, 0.0f, 1.0f, color_d3d },
  };

  render_list->add(new primitive_t(PT_LINELIST, v, 8));
}

void d3d9_renderer_t::rect_filled(float x, float y, float width, float height, color_t color) {
  D3DCOLOR color_d3d = to_d3d(color);
  vertex_t v[4] = {
    { x, y + height , 0.0f, 1.0f, color_d3d },
    { x, y, 0.0f, 1.0f, color_d3d },
    { x + width, y + height, 0.0f, 1.0f, color_d3d },
    { x + width, y, 0.0f, 1.0f, color_d3d }
  };

  render_list->add(new primitive_t(PT_TRIANGLESTRIP, v, 4));
}

void d3d9_renderer_t::rect_gradient(float x, float y, float width, float height, color_t top_left, color_t top_right, 
  color_t bottom_left, color_t bottom_right) {
  vertex_t v[] = {
    {x, y, 0.0f, 1.0f, to_d3d(top_left)},
    {x + width, y, 0.0f, 1.0f, to_d3d(top_right)},
    {x, y + height, 0.0f, 1.0f, to_d3d(bottom_left)},
    {x + width, y + height, 0.0f, 1.0f, to_d3d(bottom_right)}
  };

  render_list->add(new primitive_t(PT_TRIANGLESTRIP, v, 4));
}

inline void d3d9_renderer_t::rect_gradient(float x, float y, float w, float h, color_t left_top, color_t right_bottom,
  bool horizontal) {
  if (horizontal) rect_gradient(x, y, w, h, left_top, left_top, right_bottom, right_bottom);
  else rect_gradient(x, y, w, h, left_top, right_bottom, left_top, right_bottom);
}

void d3d9_renderer_t::rect_textured(float x, float y, texture_t* texture, color_t color) {
  const D3DCOLOR color_d3d = to_d3d(color);
  textured_vertex_t v[] = {
      { x, y + texture->height, 0.0f, 1.0f, color_d3d, 0.0f, 1.0f},
      { x, y, 0.0f, 1.0f, color_d3d, 0.0f, 0.0f},
      { x + texture->width, y + texture->height, 0.0f, 1.0f, color_d3d, 1.0f, 1.0f},
      { x + texture->width, y, 0.0f, 1.0f, color_d3d, 1.0f, 0.0f}
  };
  
  render_list->add(new primitive_t(PT_TRIANGLESTRIP, v, 4, texture));
}

// Gets the value of pi for rendering circles
inline constexpr float pi = std::numbers::pi_v<float>;

void d3d9_renderer_t::circle_filled(float x, float y, float radius, color_t color, int segments) {
  const float angle_step = 2.0f * pi / segments;
  D3DCOLOR color_d3d = to_d3d(color);
  std::vector<vertex_t> vertices(segments + 1);
  for (int i = 0; i < segments; i++) {
    float angle = i * angle_step;
    vertices[i] = { x + radius * cosf(angle), y + radius * sinf(angle), 0.0f, 1.0f, color_d3d };
  }

  vertices[segments] = { x + radius, y, 0.0f, 1.0f, color_d3d };
  
  render_list->add(new primitive_t(PT_TRIANGLEFAN, vertices.data(), segments + 1));
}

void d3d9_renderer_t::circle(float x, float y, float radius, color_t color, int segments) {
  const float angle_step = 2.0f * pi / segments;
  D3DCOLOR color_d3d = to_d3d(color);
  std::vector<vertex_t> vertices(segments + 1);
  for (int i = 0; i < segments; i++) {
    float angle = i * angle_step;
    vertices[i] = { x + radius * cosf(angle), y + radius * sinf(angle), 0.0f, 1.0f, color_d3d };
  }
  vertices[segments] = { x + radius, y, 0.0f, 1.0f, color_d3d };

  render_list->add(new primitive_t(PT_LINESTRIP, vertices.data(), segments + 1));
}

inline texture_t* d3d9_renderer_t::create_texture(int width, int height, texture_format_t format) {
  d3d9_texture_t* tex = new d3d9_texture_t();
  device->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, format == FORMAT_ALPHA ? D3DFMT_A8 : D3DFMT_A8R8G8B8, 
    D3DPOOL_DEFAULT, &tex->texture, nullptr);
  tex->width = width;
  tex->height = height;
  tex->format = format;
  tex->initialized = true;
  return tex;
}
