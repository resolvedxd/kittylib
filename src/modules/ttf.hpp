#ifndef KITTY_TTF
#define KITTY_TTF
#include <map>
#include <fstream>
#include <string>
#include "../renderer/texture.hpp"
#define STB_TRUETYPE_IMPLEMENTATION
#include "../lib/stb_truetype.h"

struct glyph_t {
  int width, height;
  int xoff, yoff;
  int index; // font glyph index
  texture_t* texture;
};

class font_t {
  private:
  // TODO: check if i can use unordered_map instead of map
  unsigned char* file_buffer;
  public:
  bool kerning = true;
  float size;
  float scale_factor; // scale factor for the provided size in pixels

  std::map<int, glyph_t> glyphs;
  stbtt_fontinfo font_info;
  renderer_t* renderer;

  font_t(const char* path, float size, renderer_t* renderer) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if(file.is_open()) {
      std::streampos size = file.tellg();
      file_buffer = new unsigned char[size];
      file.seekg(0, std::ios::beg);
      file.read(reinterpret_cast<char*>(file_buffer), size);
      file.close();
    }
    stbtt_InitFont(&font_info, file_buffer, stbtt_GetFontOffsetForIndex(file_buffer, 0));

    this->size = size;
    this->renderer = renderer;
    this->scale_factor = stbtt_ScaleForPixelHeight(&font_info, size);
  };
  ~font_t() {
    delete[] file_buffer;
  }

  void add_glyph(int codepoint) {
    int index = stbtt_FindGlyphIndex(&font_info, codepoint);
    
    int ix1, iy1, ix2, iy2;
    int ascent;
    float sub_x, sub_y;

    // get the size of the box
    stbtt_GetGlyphBitmapBox(&font_info, index, scale_factor, scale_factor, &ix1, &iy1, &ix2, &iy2);
    int width = ix2 - ix1;
    int height = iy2 - iy1;
    if (width == 0 || height == 0) return;
    // printf("%d %d %d %d (width: %d) height: (%d) char:(%c) code:(%d)\n", ix1, iy1, ix2, iy2, width, height, codepoint, codepoint);

    // create and render to a texture
    texture_t* texture = renderer->create_texture(width, height, FORMAT_ALPHA);
    access_info_t access = texture->access();
    stbtt_MakeGlyphBitmap(&font_info, (unsigned char*)access.data, width, height, access.pitch, 
      scale_factor, scale_factor, index);
    texture->commit();
    
    stbtt_GetFontVMetrics(&font_info, &ascent, 0, 0);
    int baseline = (ascent * scale_factor);

    glyphs[codepoint] = glyph_t { width, height, ix1, iy1 + (baseline), index, texture };
  }
  

  void render(const std::string text, const float x, const float y, const color_t color = color_t(255,255,255,255)) {
    int advance;
    int off = 0;
    for(int i = 0; i < text.size(); i++) {
      const int codepoint = text[i];

      // end of string
      if (codepoint == '\0') break;
      else if (codepoint == ' ') {
        off += size / 4;
        continue;
      }

      // add glyph if not already added
      if(!glyphs.contains(codepoint)) add_glyph(codepoint);
      
      const glyph_t glyph = glyphs[codepoint];
      if(!glyph.texture) continue;
      renderer->rect_textured(x + off + glyph.xoff + 0.5, y + glyph.yoff + 0.5, glyph.texture, color);

      // kerning
      if (text[i + 1] != '\0' && kerning)
        off += stbtt_GetCodepointKernAdvance(&font_info, codepoint, text[i + 1]);

      stbtt_GetGlyphHMetrics(&font_info, glyph.index, &advance, 0);
      off += scale_factor * advance;
    }
  }


  /* 
    Renders multi-colored text
    Format: "/c<HEX_COLOR>"
    Example: "/cFF0000This is some red text/cFFFFFFThis is some white text"
  */
  // render multi-colored text
  void rendermc(const std::string text, const float x, const float y, const color_t color = color_t(255,255,255,255)) {
    int advance;
    const size_t text_size = text.size();

    int off = 0;
    color_t curr_color = color;
    for(int i = 0; i < text_size; i++) {
        // printf("%c %c\n", text[i], text[i + 1]);
      if (text_size >= i + 8 && text[i] == '/' && text[i + 1] == 'c') {
        i += 2;
        curr_color = color_t::from_hex_str(text.substr(i, 6).c_str());
        i += 5;
        continue;
      }
      const int codepoint = text[i];

      // end of string
      if (codepoint == '\0') break;
      else if (codepoint == ' ') {
        off += size / 4;
        continue;
      }

      // add glyph if not already added
      if(!glyphs.contains(codepoint)) add_glyph(codepoint);
      
      const glyph_t glyph = glyphs[codepoint];
      if(!glyph.texture) continue;
      renderer->rect_textured(x + off + glyph.xoff + 0.5, y + glyph.yoff + 0.5, glyph.texture, curr_color);

      // kerning
      if (text[i + 1] != '\0' && kerning)
        off += stbtt_GetCodepointKernAdvance(&font_info, codepoint, text[i + 1]);

      stbtt_GetGlyphHMetrics(&font_info, glyph.index, &advance, 0);
      off += scale_factor * advance;
    }
  }

  vec2_t text_size(const std::string text) {
    int advance;
    int off = 0;
    for(int i = 0; i < text.size(); i++) {
      const int codepoint = text[i];

      // end of string
      if (codepoint == '\0') break;
      else if (codepoint == ' ') {
        off += size / 4;
        continue;
      }

      // add glyph if not already added
      if(!glyphs.contains(codepoint)) add_glyph(codepoint);
      
      const glyph_t glyph = glyphs[codepoint];
      if(!glyph.texture) continue;
      stbtt_GetGlyphHMetrics(&font_info, glyph.index, &advance, 0);
      off += scale_factor * advance;
    }
    return vec2_t(off, size);
  }
};
#endif