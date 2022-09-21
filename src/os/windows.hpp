#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <cstring>
#include <string>
#include "../kitty.hpp"
#include "keyboard.hpp"

namespace kitty::win32 {
  MSG msg;
  WNDPROC wndproc;
  HWND hwnd;

  HWND create_window(const char* title, int width, int height, WNDCLASSEX* wndclass = nullptr, 
    const char* classname = "test");
  kitty_msg_t handle_event(HWND hwnd = nullptr);
  inline bool should_run();
  inline bool should_render();

  std::string keycode_to_text(kitty_keycode keycode);
} // namespace kitty::win32
