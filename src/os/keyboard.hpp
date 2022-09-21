// this may change as support for more platforms is added

// The following is mostly just the windows virtual key codes
// for the sake of compatibility between different operating systems they have been copied here and will be converted to
// on operating sytems that dont use them(like linux)

// kittyboard
const enum kitty_keycode {
  KB_LBUTTON = 0x01,
  KB_RBUTTON = 0x02,
  KB_CANCEL = 0x03,
  KB_MBUTTON = 0x04,
  KB_XBUTTON1 = 0x05,
  KB_XBUTTON2 = 0x06,
  // 0x7 : reserved
  KB_BACK = 0x08,
  KB_TAB = 0x09,
  // 0x0A - 0x0B, : reserved
  KB_CLEAR = 0x0C,
  KB_RETURN = 0x0D,
  // 0x0E - 0x0F, : unassigned
  KB_SHIFT = 0x10,
  KB_CONTROL =0x11,
  KB_MENU = 0x12,
  KB_PAUSE = 0x13,
  KB_CAPITAL =0x14,
  KB_KANA = 0x15,
  KB_HANGEUL =0x15,
  KB_HANGUL = 0x15,
  KB_IME_ON = 0x16,
  KB_JUNJA = 0x17,
  KB_FINAL = 0x18,
  KB_HANJA = 0x19,
  KB_KANJI = 0x19,
  KB_IME_OFF =0x1A,
  KB_ESCAPE = 0x1B,
  KB_CONVERT =0x1C,
  KB_NONCONVERT = 0x1D,
  KB_ACCEPT = 0x1E,
  KB_MODECHANGE = 0x1F,
  KB_SPACE = 0x20,
  KB_PRIOR = 0x21,
  KB_NEXT = 0x22,
  KB_END = 0x23,
  KB_HOME = 0x24,
  KB_LEFT = 0x25,
  KB_UP = 0x26,
  KB_RIGHT = 0x27,
  KB_DOWN = 0x28,
  KB_SELECT = 0x29,
  KB_PRINT = 0x2A,
  KB_EXECUTE =0x2B,
  KB_SNAPSHOT = 0x2C,
  KB_INSERT = 0x2D,
  KB_DELETE = 0x2E,
  KB_HELP = 0x2F,
  /*
  * KB_0 - KB_9 are the same as ASCII '0' - '9' (0x30, - 0x39,)
  * 0x3A - 0x40 : unassigned
  * KB_A - KB_Z are the same as ASCII 'A' - 'Z' (0x41, - 0x5A,)
  */
  KB_0 = '0',
  KB_1 = '1',
  KB_2 = '2',
  KB_3 = '3',
  KB_4 = '4',
  KB_5 = '5',
  KB_6 = '6',
  KB_7 = '7',
  KB_8 = '8',
  KB_9 = '9',
  KB_A = 'A',
  KB_B = 'B',
  KB_C = 'C',
  KB_D = 'D',
  KB_E = 'E',
  KB_F = 'F',
  KB_G = 'G',
  KB_H = 'H',
  KB_I = 'I',
  KB_J = 'J',
  KB_K = 'K',
  KB_L = 'L',
  KB_M = 'M',
  KB_N = 'N',
  KB_O = 'O',
  KB_P = 'P',
  KB_Q = 'Q',
  KB_R = 'R',
  KB_S = 'S',
  KB_T = 'T',
  KB_U = 'U',
  KB_V = 'V',
  KB_W = 'W',
  KB_X = 'X',
  KB_Y = 'Y',
  KB_Z = 'Z',
  KB_LWIN = 0x5B,
  KB_RWIN = 0x5C,
  KB_APPS = 0x5D,
  // 0x5E, : reserved
  KB_SLEEP = 0x5F,
  KB_NUMPAD0 =0x60,
  KB_NUMPAD1 =0x61,
  KB_NUMPAD2 =0x62,
  KB_NUMPAD3 =0x63,
  KB_NUMPAD4 =0x64,
  KB_NUMPAD5 =0x65,
  KB_NUMPAD6 =0x66,
  KB_NUMPAD7 =0x67,
  KB_NUMPAD8 =0x68,
  KB_NUMPAD9 =0x69,
  KB_MULTIPLY = 0x6A,
  KB_ADD = 0x6B,
  KB_SEPARATOR = 0x6C,
  KB_SUBTRACT = 0x6D,
  KB_DECIMAL =0x6E,
  KB_DIVIDE = 0x6F,
  KB_F1 = 0x70,
  KB_F2 = 0x71,
  KB_F3 = 0x72,
  KB_F4 = 0x73,
  KB_F5 = 0x74,
  KB_F6 = 0x75,
  KB_F7 = 0x76,
  KB_F8 = 0x77,
  KB_F9 = 0x78,
  KB_F10 = 0x79,
  KB_F11 = 0x7A,
  KB_F12 = 0x7B,
  KB_F13 = 0x7C,
  KB_F14 = 0x7D,
  KB_F15 = 0x7E,
  KB_F16 = 0x7F,
  KB_F17 = 0x80,
  KB_F18 = 0x81,
  KB_F19 = 0x82,
  KB_F20 = 0x83,
  KB_F21 = 0x84,
  KB_F22 = 0x85,
  KB_F23 = 0x86,
  KB_F24 = 0x87,

  // KB_NAVIGATION_VIEW 0x88, // reserved
  // KB_NAVIGATION_MENU 0x89, // reserved
  // KB_NAVIGATION_UP 0x8A, // reserved
  // KB_NAVIGATION_DOWN 0x8B, // reserved
  // KB_NAVIGATION_LEFT 0x8C, // reserved
  // KB_NAVIGATION_RIGHT 0x8D, // reserved
  // KB_NAVIGATION_ACCEPT 0x8E, // reserved
  // KB_NAVIGATION_CANCEL 0x8F, // reserved

  KB_NUMLOCK =0x90,
  KB_SCROLL = 0x91,

  // 0x97, - 0x9F, : unassigned

  /*
  * KB_L* & KB_R* - left and right Alt, Ctrl and Shift virtual keys.
  * Used only as parameters to GetAsyncKeyState() and GetKeyState().
  * No other API or message will distinguish left and right keys in this way.
  */
  // KB_LSHIFT = 0xA0,
  // KB_RSHIFT = 0xA1,
  // KB_LCONTROL = 0xA2,
  // KB_RCONTROL = 0xA3,
  // KB_LMENU = 0xA4,
  // KB_RMENU = 0xA5,

  KB_BROWSER_BACK = 0xA6,
  KB_BROWSER_FORWARD = 0xA7,
  KB_BROWSER_REFRESH = 0xA8,
  KB_BROWSER_STOP =0xA9,
  KB_BROWSER_SEARCH = 0xAA,
  KB_BROWSER_FAVORITES = 0xAB,
  KB_BROWSER_HOME =0xAC,

  KB_VOLUME_MUTE = 0xAD,
  KB_VOLUME_DOWN = 0xAE,
  KB_VOLUME_UP = 0xAF,
  KB_MEDIA_NEXT_TRACK = 0xB0,
  KB_MEDIA_PREV_TRACK = 0xB1,
  KB_MEDIA_STOP = 0xB2,
  KB_MEDIA_PLAY_PAUSE = 0xB3,
  KB_LAUNCH_MAIL = 0xB4,
  KB_LAUNCH_MEDIA_SELECT = 0xB5,
  KB_LAUNCH_APP1 = 0xB6,
  KB_LAUNCH_APP2 = 0xB7
};
