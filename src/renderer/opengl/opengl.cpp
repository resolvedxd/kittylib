#ifdef _WIN32
#include <Windows.h>
#endif
#include <cstring>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "opengl.hpp"
#include "../../kitty.hpp"
#include "../../os/windows.hpp" // TODO: make this os-independant

opengl_renderer_t::opengl_renderer_t() {
  PIXELFORMATDESCRIPTOR pfd;
  std::memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;

  HDC hdc = GetDC(kitty::win32::hwnd);
  int pf = ChoosePixelFormat(hdc, &pfd);
  SetPixelFormat(hdc, pf, &pfd);
  DescribePixelFormat(hdc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

  this->hrc = wglCreateContext(hdc);
  wglMakeCurrent(hdc, hrc);

  // Use pixel coordinates instead of fractions
  glMatrixMode(GL_PROJECTION);
  RECT client_area;  
  GetClientRect(kitty::win32::hwnd, &client_area);
  glOrtho(0, client_area.right, client_area.bottom, 0, -1, 1);

  // Enabled the use of a vertex array
  glEnableClientState(GL_VERTEX_ARRAY);
}

bool opengl_renderer_t::uninitialize() {
  wglMakeCurrent(nullptr, nullptr);
  wglDeleteContext(this->hrc);
  return true;
}

void opengl_renderer_t::begin() {
  glClear(GL_COLOR_BUFFER_BIT);
}

void opengl_renderer_t::end() {
  for (auto renderable : render_list->renderables) {
      switch (renderable->renderable_type) {
      case PRIMITIVE:
      {
        primitive_t* primitive = (primitive_t*)renderable;
        // printf("primitive type: %d count: %d\n", primitive->primitive_type, primitive->primitive_count());
        glVertexPointer(primitive->primitive_count(), GL_FLOAT, 0, primitive->vertices.data());
        glDrawArrays(primitive->opengl_primitive(), 0, primitive->primitive_count());
      }
    }
  }
  // glVertexPointer(2, GL_FLOAT, 0, this->render_list->renderables.data());
}

void opengl_renderer_t::line(float x1, float y1, float x2, float y2, color_t color) {
  render_list->add();
}