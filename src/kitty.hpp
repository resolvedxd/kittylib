#pragma once
#include "renderer/renderer.hpp"

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

namespace kitty {
  namespace os {
    void create_window(const char* title, int width, int height);
    kitty_msg_t handle_event();
    bool should_run();
    bool should_render();
    int message_box(const char* title, const char* message, kitty_messagebox_type type = MESSAGEBOX_OK);

    int cursor_x, cursor_y;
    bool hide_cursor;
  }

  void error(const char* message);
}

#include "os/os.hpp"