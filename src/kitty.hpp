#pragma once
#include "os/os.hpp"
#include "renderer/renderer.hpp"

namespace kitty {
  void error(const char* message) {
    fprintf(stderr, "ERROR! %s\n", message);
    exit(1);
  }
}

// #include "os/os.cpp"