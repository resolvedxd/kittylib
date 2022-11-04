#pragma once

enum kitty_messagebox_type {
  MESSAGEBOX_OK,
  MESSAGEBOX_YESNO,
  MESSAGEBOX_ERROR,
};

enum msg_type {
  UNKNOWN_EVENT,
  MOUSE_MOVE,
  MOUSE_LEFTCLICK,
  MOUSE_MIDDLECLICK,
  MOUSE_RIGHTCLICK,
  KEY_DOWN,
  KEY_UP,
};

struct kitty_msg_t {
  msg_type type;
  int a1;
  int a2;
};

namespace kitty::os {
    void create_window(const char* title, int width, int height);
    kitty_msg_t handle_event();
    bool should_run();
    bool should_render();
    int message_box(const char* title, const char* message, kitty_messagebox_type type = MESSAGEBOX_OK);
    void error(const char* message);

    inline int cursor_x, cursor_y;
    inline bool hide_cursor;

    // this function will be called when a render is requested but the main thread is being blocked,
    // for example when the window is being moved or resized. the window size is passed in the parameters, so you can
    // change the renderer viewport to match the window size. 
    // (implementing this is entirly optional)
    inline void(*request_render)(int width, int height);
}

#ifdef _WIN32
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include "keyboard.hpp"
namespace kitty::win32 {
  inline MSG msg;
  inline WNDPROC wndproc;
  inline HWND hwnd;

  HWND create_window(const char* title, int width, int height, WNDCLASSEX* wndclass = nullptr, 
    const char* classname = "test");
  kitty_msg_t handle_event(HWND hwnd = nullptr);
  bool should_run();
  bool should_render();

  std::string keycode_to_text(kitty_keycode keycode);
} // namespace kitty::win32
#endif _WIN32