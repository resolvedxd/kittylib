# kittylib - The cutest C++ graphics library
**kitty** is a **C++** graphics library that provides a **simple** and **easy to use** API to create and render 2D graphics. The library is ~~cross-platform~~(not yet), **optimized for speed** and **memory efficiency**.

## Examples
### creating a window and rendering text to it
```cpp
kitty::os::create_window("Kittylib", 800, 600);
renderer_t* renderer = new d3d9_renderer_t();
font_t* font = new font_t("C:\\Windows\\Fonts\\arial.ttf", 25, renderer);
while (kitty::os::should_run()) {
  font->render("Hello World", 0, 0, color_t(255, 255, 255, 255));
}
```
### working with the color class
```cpp
// every way to initialize a color (conversions back and forth from these color models are supported)
color_t color1(255, 0, 0);
color_t color2 = color_t::red();
color_t color3 = color_t::from_hex_str("#FF0000");
color_t color4 = colot_t::from_hsl(313, 1, 0.81);
color_t color5 = colot_t::from_hsv(313, 0.38, 1.0);

// calculate a color's luma
char luma = color1.luma();

// interpolate between two colors
color_t color6 = color2.lerp(color4, 0.5);
```
### off-screen rendering
```cpp
// create a texture
texture_t* texture = new texture_t(100, 100, renderer);
renderer->begin_texture(texture); 
  // render stuff to the texture
  renderer->circle_filled(0, 0, 100, color_t::white());
  font->render("Hi", 50, 50, color_t::white());
renderer->end_texture();

// now we can use the texture we rendered to as a sprite etc.
renderer->rect_textured(0, 0, texture);
```
### and many more features, such as: 
- **2D shapes** (rectangles, circles, triangles, lines, polygons, etc.)
- **easy to use vector,matrice classes** and a math namespace for common operations (such as bezier, clamping, mapping, etc.)
- **image loading** (png, jpg, bmp, etc.)
- **OS message handling** (keyboard, mouse, etc.)
- **and more! 🎉**

## Wiki and documentation TBD

# TODO
- [x] D3D9 Renderer
- [ ] OpenGL renderer
- [x] Textures
- [x] Font renderer
- [ ] Free color_t conversion to UINT32
- [x] Fix crashing when alot of frames are being rendered in a short period of time
- [ ] Look into faster ways to calculate bezier curves