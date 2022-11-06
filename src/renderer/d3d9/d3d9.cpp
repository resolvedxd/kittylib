#include <cstdio>
#include <cstring>
#include <numbers>
#include <math.h>
#include <vector>
#include <d3d9.h>
#include "d3d9.hpp"
#include "../../utils/math.hpp"

d3d9_renderer_t::d3d9_renderer_t() {
  LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);
  
  std::memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
  d3dpp.Windowed = true;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.hDeviceWindow = kitty::win32::hwnd;
  d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
  d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
  d3dpp.MultiSampleQuality = 0;

  HRESULT hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, kitty::win32::hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, 
    &this->device);

  if (!this->initialize(this->device)) {
    printf("Failed to initialize d3d9 renderer 0x%x\n", hr);
    exit(1);
  }
}

bool d3d9_renderer_t::initialize(IDirect3DDevice9* device) {
  if (!device) return false;
  this->device = device;
  this->render_list = new render_list_t();
  return true;
}

bool is_rendering = false;
void d3d9_renderer_t::begin() {
  if (is_rendering) printf("warning: begin() called while already rendering\n");
  is_rendering = true;
  device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

  // anti aliasing
  device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, antialias);
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
  if (!is_rendering) printf("warning: end() called while not rendering\n");
  is_rendering = false;
  for (auto renderable : render_list->renderables) {
    switch (renderable->renderable_type) {
      case PRIMITIVE: {
        primitive_t<vertex_t>* primitive = (primitive_t<vertex_t>*)renderable;
        device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
        device->DrawPrimitiveUP((D3DPRIMITIVETYPE)primitive->primitive_type, primitive->primitive_count(), 
          primitive->vertices.data(), sizeof(vertex_t));
        break;
      }
      case TEXTURE: {
        primitive_t<textured_vertex_t>* primitive = (primitive_t<textured_vertex_t>*)renderable;
        d3d9_texture_t* texture = (d3d9_texture_t*)primitive->texture;

        device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2);
        device->SetTexture(0, texture->texture);

        if (texture->format == FORMAT_ALPHA)
          device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);

        device->DrawPrimitiveUP((D3DPRIMITIVETYPE)primitive->primitive_type, primitive->primitive_count(), 
          primitive->vertices.data(), sizeof(textured_vertex_t));

        // if (primitive->texture->format == FORMAT_ALPHA)
        device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        
        device->SetTexture(0, nullptr);
        device->SetTexture(1, nullptr);
        device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
        break;
      }
      case MASK: {
        primitive_t<two_textured_vertex_t>* primitive = (primitive_t<two_textured_vertex_t>*)renderable;
        d3d9_texture_t* texture = (d3d9_texture_t*)primitive->texture;
        d3d9_texture_t* mask = (d3d9_texture_t*)texture->mask;

        device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2);
        device->SetTexture(0, texture->texture);
        device->SetTexture(1, mask->texture);

        device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

        // Use the color from the previous texture, and blend the alpha from the mask.
        device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
        device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        device->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        device->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

        device->DrawPrimitiveUP((D3DPRIMITIVETYPE)primitive->primitive_type, primitive->primitive_count(), 
          primitive->vertices.data(), sizeof(two_textured_vertex_t));

        device->SetTexture(0, nullptr);
        device->SetTexture(1, nullptr);
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

  render_list->add(new primitive_t<std::remove_reference<decltype(*v)>::type>(PT_LINELIST, v, 2));
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

  render_list->add(new primitive_t<std::remove_reference<decltype(*v)>::type>(PT_TRIANGLESTRIP, v, 4));
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

  render_list->add(new primitive_t<std::remove_reference<decltype(*v)>::type>(PT_LINELIST, v, 8));
}

void d3d9_renderer_t::rect_filled(float x, float y, float width, float height, color_t color) {
  D3DCOLOR color_d3d = to_d3d(color);
  vertex_t v[4] = {
    { x, y + height , 0.0f, 1.0f, color_d3d },
    { x, y, 0.0f, 1.0f, color_d3d },
    { x + width, y + height, 0.0f, 1.0f, color_d3d },
    { x + width, y, 0.0f, 1.0f, color_d3d }
  };

  render_list->add(new primitive_t<std::remove_reference<decltype(*v)>::type>(PT_TRIANGLESTRIP, v, 4));
}

void d3d9_renderer_t::rect_gradient(float x, float y, float width, float height, color_t top_left, color_t top_right, 
  color_t bottom_left, color_t bottom_right) {
  vertex_t v[] = {
    {x, y, 0.0f, 1.0f, to_d3d(top_left)},
    {x + width, y, 0.0f, 1.0f, to_d3d(top_right)},
    {x, y + height, 0.0f, 1.0f, to_d3d(bottom_left)},
    {x + width, y + height, 0.0f, 1.0f, to_d3d(bottom_right)}
  };

  render_list->add(new primitive_t<std::remove_reference<decltype(*v)>::type>(PT_TRIANGLESTRIP, v, 4));
}

inline void d3d9_renderer_t::rect_gradient(float x, float y, float w, float h, color_t left_top, color_t right_bottom,
  bool horizontal) {
  if (horizontal) rect_gradient(x, y, w, h, left_top, left_top, right_bottom, right_bottom);
  else rect_gradient(x, y, w, h, left_top, right_bottom, left_top, right_bottom);
}

void d3d9_renderer_t::rect_textured(float x, float y, texture_t* texture, color_t color) {
  const D3DCOLOR color_d3d = to_d3d(color);

  if (texture->alpha_masked) {
    two_textured_vertex_t v[] = {
      { x, y + texture->height, 0.0f, 1.0f, color_d3d, 0.0f, 1.0f, 0.0f, 1.0f},
      { x, y, 0.0f, 1.0f, color_d3d, 0.0f, 0.0f, 0.0f, 0.0f},
      { x + texture->width, y + texture->height, 0.0f, 1.0f, color_d3d, 1.0f, 1.0f, 1.0f, 1.0f},
      { x + texture->width, y, 0.0f, 1.0f, color_d3d, 1.0f, 0.0f, 1.0f, 0.0f}
    };
    render_list->add(new primitive_t<std::remove_reference<decltype(*v)>::type>(PT_TRIANGLESTRIP, v, 4, texture));
  } else {
    textured_vertex_t v[] = {
        { x, y + texture->height, 0.0f, 1.0f, color_d3d, 0.0f, 1.0f},
        { x, y, 0.0f, 1.0f, color_d3d, 0.0f, 0.0f},
        { x + texture->width, y + texture->height, 0.0f, 1.0f, color_d3d, 1.0f, 1.0f},
        { x + texture->width, y, 0.0f, 1.0f, color_d3d, 1.0f, 0.0f}
    };
    render_list->add(new primitive_t<std::remove_reference<decltype(*v)>::type>(PT_TRIANGLESTRIP, v, 4, texture));
  }
}

// Gets the value of pi for rendering circles
inline constexpr float pi = std::numbers::pi_v<float>;

static inline constexpr void rect_rounded_verts(std::vector<vertex_t> &v, float x, float y, float width, float height, 
    float rad_leftup, float rad_rightup, float rad_leftdown, float rad_rightdown,
    D3DCOLOR color_d3d, int segments, float smoothness, bool multisample_offset) {
  for (int i = 0; i < segments; i++) {
    float p0[] = { x, y + rad_leftup  };
    float p1[] = { x, y + rad_leftup * smoothness };
    float p2[] = { x + rad_leftup * smoothness, y };
    float p3[] = { x + rad_leftup, y };
    if (multisample_offset) {
      p0[0] += 0.5f;
      p3[1] += 0.5f;
    }

    v[i] = {
      kmath::bezier(p0[0], p1[0], p2[0], p3[0], i / (float)segments),
      kmath::bezier(p0[1], p1[1], p2[1], p3[1], i / (float)segments),
      0.0f, 1.0f, color_d3d
    };

    p0[0] = x + width - rad_rightup; p0[1] = y;
    p1[0] = x + width - rad_rightup * smoothness; p1[1] = y;
    p2[0] = x + width; p2[1] = y + rad_rightup * smoothness;
    p3[0] = x + width; p3[1] = y + rad_rightup;
    if (multisample_offset) {
      p0[1] += 0.5f;
      p3[0] -= 0.5f;
    }

    v[i + segments] = {
      kmath::bezier(p0[0], p1[0], p2[0], p3[0], i / (float)segments),
      kmath::bezier(p0[1], p1[1], p2[1], p3[1], i / (float)segments),
      0.0f, 1.0f, color_d3d
    };

    p0[0] = x + width; p0[1] = y + height - rad_rightdown;
    p1[0] = x + width; p1[1] = y + height - rad_rightdown * smoothness;
    p2[0] = x + width - rad_rightdown * smoothness; p2[1] = y + height;
    p3[0] = x + width - rad_rightdown; p3[1] = y + height;
    if (multisample_offset) {
      p0[0] -= 0.5f;
      p3[1] -= 0.5f;
    }

    v[i + segments * 2] = {
      kmath::bezier(p0[0], p1[0], p2[0], p3[0], i / (float)segments),
      kmath::bezier(p0[1], p1[1], p2[1], p3[1], i / (float)segments),
      0.0f, 1.0f, color_d3d
    };

    p0[0] = x + rad_leftdown; p0[1] = y + height;
    p1[0] = x + rad_leftdown * smoothness; p1[1] = y + height;
    p2[0] = x; p2[1] = y + height - rad_leftdown * smoothness;
    p3[0] = x; p3[1] = y + height - rad_leftdown;
    if (multisample_offset) {
      p0[1] -= 0.5f;
      p3[0] += 0.5f;
    }

    v[i + segments * 3] = {
      kmath::bezier(p0[0], p1[0], p2[0], p3[0], i / (float)segments),
      kmath::bezier(p0[1], p1[1], p2[1], p3[1], i / (float)segments),
      0.0f, 1.0f, color_d3d
    };
  }
}

void d3d9_renderer_t::rect_rounded(float x, float y, float width, float height, float rad_leftup, float rad_rightup, 
float rad_leftdown, float rad_rightdown, color_t color, int segments, float smoothness) {
  D3DCOLOR color_d3d = to_d3d(color);
  std::vector<vertex_t> v(segments * 4 + 1);

  rect_rounded_verts(v, x, y, width, height, rad_leftup, rad_rightup, rad_leftdown, rad_rightdown, color_d3d, segments, 
  smoothness, false);

  v[segments * 4] = v[0];

  render_list->add(new primitive_t<vertex_t>(PT_LINESTRIP, v.data(), v.size()));
}

void d3d9_renderer_t::rect_rounded_filled(float x, float y, float width, float height, float rad_leftup, 
float rad_rightup, float rad_leftdown, float rad_rightdown, color_t color, int segments, float smoothness) {
  D3DCOLOR color_d3d = to_d3d(color);
  std::vector<vertex_t> v(segments * 4);

  rect_rounded_verts(v, x, y, width, height, rad_leftup, rad_rightup, rad_leftdown, rad_rightdown, color_d3d, segments, 
  smoothness, true);

  render_list->add(new primitive_t<vertex_t>(PT_TRIANGLEFAN, v.data(), v.size()));
}

void d3d9_renderer_t::circle_filled(float x, float y, float radius, color_t color, int segments) {
  const float angle_step = 2.0f * pi / segments;
  D3DCOLOR color_d3d = to_d3d(color);
  std::vector<vertex_t> v(segments + 1);
  for (int i = 0; i < segments; i++) {
    float angle = i * angle_step;
    v[i] = { x + radius * cosf(angle), y + radius * sinf(angle), 0.0f, 1.0f, color_d3d };
  }

  v[segments] = { x + radius, y, 0.0f, 1.0f, color_d3d };
  
  // TODO: evaluate the type of the vector for the primitive_t template automatically
  render_list->add(new primitive_t<vertex_t>(PT_TRIANGLEFAN, v.data(), segments + 1));
}

void d3d9_renderer_t::circle(float x, float y, float radius, color_t color, int segments) {
  const float angle_step = 2.0f * pi / segments;
  D3DCOLOR color_d3d = to_d3d(color);
  std::vector<vertex_t> v(segments + 1);
  for (int i = 0; i < segments; i++) {
    float angle = i * angle_step;
    v[i] = { x + radius * cosf(angle), y + radius * sinf(angle), 0.0f, 1.0f, color_d3d };
  }
  v[segments] = { x + radius, y, 0.0f, 1.0f, color_d3d };

  // TODO: evaluate the type of the vector for the primitive_t template automatically
  render_list->add(new primitive_t<vertex_t>(PT_LINESTRIP, v.data(), segments + 1));
}

inline texture_t* d3d9_renderer_t::create_texture(int width, int height, texture_format_t format) {
  d3d9_texture_t* tex = new d3d9_texture_t();
  if (FAILED(device->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, 
  format == FORMAT_ALPHA ? D3DFMT_A8 : D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &tex->texture, nullptr)))
    kitty::os::error("Failed to create texture");
  tex->width = width;
  tex->height = height;
  tex->format = format;
  tex->initialized = true;
  textures.push_back(tex);
  return tex;
}

// NOTE: all of the following functions may fail with no error message if improper arguments are used
// TODO: add error checking
IDirect3DTexture9* render_target_texture = nullptr;
IDirect3DSurface9* texture_surface;
IDirect3DSurface9* screen_surface;
void d3d9_renderer_t::begin_texture(int width, int height, texture_format_t format) {
  device->GetRenderTarget(0, &screen_surface);
  device->CreateTexture(width, height, 0, D3DUSAGE_RENDERTARGET, format == FORMAT_ALPHA ? D3DFMT_A8 : D3DFMT_A8R8G8B8, 
    D3DPOOL_DEFAULT, &render_target_texture, NULL);
  render_target_texture->GetSurfaceLevel(0, &texture_surface);
  device->SetRenderTarget(0, texture_surface);

  begin();
}

void d3d9_renderer_t::begin_texture(texture_t* texture) {
  if (!((d3d9_texture_t*)texture)->texture) return;
  if (FAILED(device->GetRenderTarget(0, &screen_surface)) || 
  FAILED(((d3d9_texture_t*)texture)->texture->GetSurfaceLevel(0, &texture_surface)))
    kitty::os::error("Failed to get render target");
  device->SetRenderTarget(0, texture_surface);

  begin();
}

// returns nothing if begin_texture was called with a texture_t* argument
texture_t* d3d9_renderer_t::end_texture() {
  end();

  device->SetRenderTarget(0, screen_surface);
  screen_surface->Release();
  
  // return prematurely if begin_texture was called with a texture_t* argument
  if (!render_target_texture) {
    render_target_texture = nullptr;
    return nullptr;
  }

  // create texture
  D3DSURFACE_DESC desc;
  render_target_texture->GetLevelDesc(0, &desc);
  d3d9_texture_t* tex = new d3d9_texture_t();
  tex->texture = render_target_texture;
  tex->width = desc.Width;
  tex->height = desc.Height;
  tex->format = desc.Format == D3DFMT_A8 ? FORMAT_ALPHA : FORMAT_ARGB;
  tex->initialized = true;
  
  render_target_texture = nullptr;
  
  return tex;
}

// non-functional
void d3d9_renderer_t::set_viewport(int x, int y, int width, int height) {
  this->viewport.X = x;
  this->viewport.Y = y;
  this->viewport.Width = width;
  this->viewport.Height = height;

  // unfortunetly, d3d9 is retarded
  // d3dpp.BackBufferWidth = width;
  // d3dpp.BackBufferHeight = height;
  // for (auto& texture : textures) {
  //   if (texture->initialized) texture->texture->Release();
  // }
  // device->Reset(&d3dpp);
  // for (auto& texture : textures) {
  //   device->CreateTexture(texture->width, texture->height, 1, D3DUSAGE_DYNAMIC, 
  //   texture->format == FORMAT_ALPHA ? D3DFMT_A8 : D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture->texture, nullptr);
  //   // texture->initialized = false;
  // }
}