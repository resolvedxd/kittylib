// #pragma once
#include <vector>
#include <typeinfo>
#include "../renderer/texture.hpp"
// #include "../renderer/d3d9/d3d9_tex_impl.hpp"

struct vertex_t {
  float x, y, z, rhw;
  unsigned long color;
};

struct textured_vertex_t {
  float x, y, z, rhw;
  unsigned long color;
  float u, v; // texture coordinates
};

struct two_textured_vertex_t {
  float x, y, z, rhw;
  unsigned long color;
  float u1, v1; // texture1 coordinates
  float u2, v2; // texture2 coordinates
};

enum renderable_type_t {
  PRIMITIVE,
  TEXTURE,
  MASK, // two textures
};

class renderable_t {
  public:
  renderable_type_t renderable_type;
};

// a template that allows us to store different types of vertexes in the same vector
template <typename T>
class primitive_t : public renderable_t {
  public:
  primitive_t(primitive_type_t type, T* pvertices, int vertex_count, texture_t* texture = nullptr) {
    if (typeid(T) == typeid(vertex_t)) {
      this->renderable_type = PRIMITIVE;
    } else if (typeid(T) == typeid(textured_vertex_t)) {
      this->renderable_type = TEXTURE;
    } else if (typeid(T) == typeid(two_textured_vertex_t)) {
      this->renderable_type = MASK;
    }
    this->primitive_type = type;
    
    this->vertices.resize(vertex_count);
    std::memcpy(this->vertices.data(), pvertices, sizeof(T) * vertex_count);
    this->vertex_count = vertex_count;

    this->texture = texture;
  };
  ~primitive_t() {
    vertices.clear();

    // if (texture) {
    //   if (typeid(*texture) == typeid(d3d9_texture_t)) {
    //     printf("d3d9 texture\n");
    //     //  ((d3d9_texture_t*)texture)->destroy();
    //   }
    // }
    // delete texture;
  };
  primitive_type_t primitive_type;
  std::vector<T> vertices;
  int vertex_count;
  texture_t* texture;

  int primitive_count() {
    switch (primitive_type) {
      case PT_TRIANGLEFAN:
        return vertex_count - 2;
      case PT_TRIANGLESTRIP:
        return vertex_count - 2;
      case PT_TRIANGLELIST:
        return vertex_count / 3;
      case PT_LINESTRIP:
        return vertex_count - 1;
      case PT_LINELIST:
        return vertex_count / 2;
      default:
        return vertex_count;
    };

    return 0;
  };

  const inline int opengl_primitive() {
    #define GL_POINTS                         0x0000
    #define GL_LINES                          0x0001
    #define GL_LINE_LOOP                      0x0002
    #define GL_LINE_STRIP                     0x0003
    #define GL_TRIANGLES                      0x0004
    #define GL_TRIANGLE_STRIP                 0x0005
    #define GL_TRIANGLE_FAN                   0x0006
    #define GL_QUADS                          0x0007
    #define GL_QUAD_STRIP                     0x0008
    #define GL_POLYGON                        0x0009
    switch (primitive_type) {
      case PT_POINTLIST:
        return GL_POINTS;
      case PT_LINESTRIP:
        return GL_LINE_STRIP;
      case PT_LINELIST:
        return GL_LINES;
      case PT_TRIANGLEFAN:
        return GL_TRIANGLE_FAN;
      case PT_TRIANGLESTRIP:
        return GL_TRIANGLE_STRIP;
      case PT_TRIANGLELIST:
        return GL_TRIANGLES;
      default:
        return GL_POINTS;
    };
  };
}; // class primitive

class render_list_t {
  public:
  std::vector<renderable_t*> renderables;

  void add(primitive_t<vertex_t>* primitive) {
    renderables.push_back(primitive);
  }
  void add(primitive_t<textured_vertex_t>* primitive) {
    renderables.push_back(primitive);
  }
  void add(primitive_t<two_textured_vertex_t>* primitive) {
    renderables.push_back(primitive);
  }
  void empty() {
    for(auto renderable : renderables) {
      // if (typeid(*renderable) == typeid(primitive_t<vertex_t>)) {
      //   delete (primitive_t<vertex_t>*)renderable;
      // } else if (typeid(*renderable) == typeid(primitive_t<textured_vertex_t>)) {
      //   delete (primitive_t<textured_vertex_t>*)renderable;
      // } else if (typeid(*renderable) == typeid(primitive_t<two_textured_vertex_t>)) {
      //   delete (primitive_t<two_textured_vertex_t>*)renderable;
      // } else {
      //   // i guess just assume its the biggest type available... (this is not safe)
      //   delete (primitive_t<two_textured_vertex_t>*)renderable;
      // }
      delete (primitive_t<two_textured_vertex_t>*)renderable;
    }
    renderables.clear();
  }
}; // class render_list
