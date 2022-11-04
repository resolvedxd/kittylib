#include <cstring>
#include "os.hpp"

bool allow_hiding_cursor = true;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_MOVE:
      if (kitty::os::request_render) kitty::os::request_render(0, 0);
      break;
    case WM_SIZE:
      if (kitty::os::request_render) kitty::os::request_render(LOWORD(lParam), HIWORD(lParam));
      break;
    // case WM_SIZING:
    //   if (kitty::os::request_render && lParam) 
    //     kitty::os::request_render(
    //       ((RECT*)lParam)->right - ((RECT*)lParam)->left, 
    //       ((RECT*)lParam)->bottom - ((RECT*)lParam)->top);
    //   break;
    

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_SETCURSOR:
      if (allow_hiding_cursor && kitty::os::hide_cursor) {
        SetCursor(nullptr);
        return TRUE;
      }
      break;

    // Shows the cursor when the mouse is not in the client area
    case WM_NCMOUSEMOVE:
      allow_hiding_cursor = false;
      ShowCursor(true);
      break;
    case WM_MOUSEMOVE:
      allow_hiding_cursor = true;
      break;
  }

  return DefWindowProc(hwnd, message, wParam, lParam);
};

HWND kitty::win32::create_window(const char* title, int width, int height, WNDCLASSEX* wndclass, 
    const char* classname) {
  kitty::win32::wndproc = WindowProc;
  kitty::win32::msg.message = 0;

  WNDCLASSEXA wc;
  std::memset(&wc, 0, sizeof(WNDCLASSEXA));
  wc.cbSize = sizeof(WNDCLASSEXA);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = kitty::win32::wndproc;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpszClassName = classname;
  
  RegisterClassExA(&wc);

  kitty::win32::hwnd = CreateWindowA(classname, title, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
  if (!kitty::win32::hwnd) {
    kitty::os::error("Failed to create window");
    return nullptr;
  }
  
  ShowWindow(kitty::win32::hwnd, SW_SHOW);
  #ifndef _DEBUG
  FreeConsole();
  #else
  SetConsoleTitleA("Kitty Debug Console");
  #endif
  return kitty::win32::hwnd;
}

#define update_cursor_pos(msg) kitty::os::cursor_x = GET_X_LPARAM(msg.lParam);\
                                kitty::os::cursor_y = GET_Y_LPARAM(msg.lParam);

kitty_msg_t kitty::win32::handle_event(HWND hwnd) {
  MSG msg;
  PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
  TranslateMessage(&msg);
  DispatchMessage(&msg);


  kitty::win32::msg = msg;

  switch (msg.message) {
    // Mouse events
    case WM_MOUSEMOVE:
      update_cursor_pos(msg);
      return kitty_msg_t { MOUSE_MOVE, kitty::os::cursor_x, kitty::os::cursor_y };
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
      update_cursor_pos(msg);
      return kitty_msg_t { MOUSE_LEFTCLICK, kitty::os::cursor_x, kitty::os::cursor_y };
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
      update_cursor_pos(msg);
      return kitty_msg_t { MOUSE_MIDDLECLICK, kitty::os::cursor_x, kitty::os::cursor_y };
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
      update_cursor_pos(msg);
      return kitty_msg_t { MOUSE_RIGHTCLICK, kitty::os::cursor_x, kitty::os::cursor_y };
    
    // Keyboard events
    case WM_KEYDOWN:
      return kitty_msg_t { KEY_DOWN, (kitty_keycode)msg.wParam, 0 };
    case WM_KEYUP:
      return kitty_msg_t { KEY_UP, (kitty_keycode)msg.wParam, 0 };

    default:
      return kitty_msg_t { UNKNOWN_EVENT, (int)msg.lParam, (int)msg.wParam };
  }
}

bool kitty::win32::should_run() {
  return kitty::win32::msg.message != WM_QUIT;
}

bool kitty::win32::should_render() {
  return true/*kitty::win32::msg.message == WM_PAINT*/;
}

std::string kitty::win32::keycode_to_text(kitty_keycode keycode) {
  char* lpString = new char[64];

  switch (keycode) {
    case 0:
      return "None";

    case KB_LBUTTON:
      return "Left click";

    case KB_RBUTTON:
      return "Right click";

    case KB_MBUTTON:
      return "Middle click";

    default: {
      unsigned int scancode = MapVirtualKey(keycode, MAPVK_VK_TO_VSC);
      int lParam = (scancode << 16);
      GetKeyNameTextA(lParam, lpString, 64);
    }
  }

  return std::string(lpString);
}