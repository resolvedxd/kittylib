#ifndef KITTY_IMG
#define KITTY_IMG
#include "../renderer/texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

namespace kitty::img {
  namespace {
    static inline texture_t* create_texture(renderer_t* renderer, const int width, const int height, unsigned char* img) {
      texture_t* texture = renderer->create_texture(width, height);
      const access_info_t access = texture->access();
      unsigned char* data = (unsigned char*)access.data;

      for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
          unsigned int offset = (y * width + x) * 4;
          unsigned char* img_pixel = img + offset;
          unsigned char* data_pixel = data + (access.pitch * y) + (x * 4);

          // convert from RGBA to ARGB
          data_pixel[3] = img_pixel[3];
          data_pixel[2] = img_pixel[0];
          data_pixel[1] = img_pixel[1];
          data_pixel[0] = img_pixel[2];
        }
      }
      texture->commit();

      return texture;
    }
  }

  texture_t* load(renderer_t* renderer, const char* path) {
    int width = 0, height = 0, nr_channels = 0;

    unsigned char* img = stbi_load(path, &width, &height, &nr_channels, 4);
    if (img == nullptr) return nullptr;

    texture_t* texture = create_texture(renderer, width, height, img);

    stbi_image_free(img);
    return texture;
  }

  texture_t* load_from_memory(renderer_t* renderer, unsigned char* data, int size) {
    int width = 0, height = 0, nr_channels = 0;

    unsigned char* img = stbi_load_from_memory(data, size, &width, &height, &nr_channels, 4);
    if (img == nullptr) return nullptr;

    texture_t* texture = create_texture(renderer, width, height, img);

    stbi_image_free(img);
    return texture;
  }
}
#endif