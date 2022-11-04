#pragma once
#include <cstdio>
#include "os.hpp"

// TODO: add support for more os's
void kitty::os::create_window(const char* title, int width, int height) {
    #ifdef _WIN32
    kitty::win32::create_window(title, width, height);
    #endif
}
  
kitty_msg_t kitty::os::handle_event() {
  #ifdef _WIN32
  return kitty::win32::handle_event();
  #endif
}

bool kitty::os::should_run() {
  #ifdef _WIN32
  return kitty::win32::should_run();
  #endif
}

bool kitty::os::should_render() {
  #ifdef _WIN32
  return kitty::win32::should_render();
  #endif
  return true;
}

int kitty::os::message_box(const char* title, const char* message, kitty_messagebox_type type) {
  #ifdef _WIN32
  return MessageBoxA(nullptr, message, title, 
    type == MESSAGEBOX_YESNO ? MB_YESNO : MB_OK | 
    type == MESSAGEBOX_ERROR ? MB_ICONERROR : 0
  );
  #endif
}

void kitty::os::error(const char* message) {
  kitty::os::message_box("Error", message, MESSAGEBOX_ERROR);
  printf("ERROR! %s\n", message);
  exit(1);
}
