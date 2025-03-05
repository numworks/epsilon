#ifndef EADKPP_H
#define EADKPP_H

extern "C" {

#include <eadk.h>
extern const char eadk_app_name[];
extern const uint32_t eadk_api_level;

}

namespace EADK {

class Color {
public:
  constexpr Color(uint32_t rgb) : m_value(((rgb&0xF80000)>>8)|((rgb&0x00FC00)>>5)|((rgb&0x0000F8)>>3)) {}
  constexpr operator eadk_color_t() const { return (eadk_color_t)m_value; }
private:
  uint16_t m_value;
};
static_assert(sizeof(EADK::Color) == sizeof(eadk_color_t), "EADK::Color should match eadk_color_t");

class Point {
public:
  constexpr Point(int x, int y) :
    m_x(x), m_y(y) {}
  uint16_t x() const { return m_x; }
  uint16_t y() const { return m_y; }
  operator eadk_point_t() const { return *reinterpret_cast<const eadk_point_t *>(this); }
private:
  uint16_t m_x;
  uint16_t m_y;
};
static_assert(sizeof(EADK::Point) == sizeof(eadk_point_t), "EADK::Point should match eadk_point_t");

class Rect {
public:
  constexpr Rect(int x, int y, int width, int height) :
    m_x(x), m_y(y), m_width(width), m_height(height) {}
  uint16_t x() const { return m_x; }
  uint16_t y() const { return m_y; }
  uint16_t width() const { return m_width; }
  uint16_t height() const { return m_height; }
  operator eadk_rect_t() const { return *reinterpret_cast<const eadk_rect_t *>(this); }
private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_width;
  uint16_t m_height;
};
static_assert(sizeof(EADK::Rect) == sizeof(eadk_rect_t), "EADK::Rect should match eadk_rect_t");

namespace Screen {
  constexpr uint16_t Width = EADK_SCREEN_WIDTH;
  constexpr uint16_t Height = EADK_SCREEN_HEIGHT;
  constexpr Rect Rect(0, 0, Width, Height);
}

namespace Display {

static inline void pushRect(Rect rect, const Color * pixels) {
  eadk_display_push_rect(rect, reinterpret_cast<const eadk_color_t *>(pixels));
}

static inline void pushRectUniform(Rect rect, Color color) {
  eadk_display_push_rect_uniform(rect, color);
}
static inline void drawString(const char * text, Point point, bool largeFont, Color textColor, Color backgroundColor) {
  eadk_display_draw_string(text, point, largeFont, textColor, backgroundColor);
}

}

namespace Keyboard {

enum class Key : uint8_t {
  Left = 0,
  Up = 1,
  Down = 2,
  Right = 3,
  OK = 4,
  Back = 5,
  Home = 6,
  Shift = 12,
  Alpha = 13,
  XNT = 14,
  Var = 15,
  Toolbox = 16,
  Backspace = 17,
  Exp = 18,
  Ln = 19,
  Log = 20,
  Imaginary = 21,
  Comma = 22,
  Power = 23,
  Sine = 24,
  Cosine = 25,
  Tangent = 26,
  Pi = 27,
  Sqrt = 28,
  Square = 29,
  Seven = 30,
  Eight = 31,
  Nine = 32,
  LeftParenthesis = 33,
  RightParenthesis = 34,
  Four = 36,
  Five = 37,
  Six = 38,
  Multiplication = 39,
  Division = 40,
  One = 42,
  Two = 43,
  Three = 44,
  Plus = 45,
  Minus = 46,
  Zero = 48,
  Dot = 49,
  EE = 50,
  Ans = 51,
  EXE = 52,
};

enum class Event : uint8_t {
  Left = 0,
  Up = 1,
  Down  = 2,
  Right = 3,
  Ok    = 4,
  Back  = 5,
  Shift = 12,
  Alpha = 13,
  Xnt   = 14,
  Var   = 15,
  Toolbox = 16,
  Backspace = 17,
  Exp = 18,
  Ln = 19,
  Log = 20,
  Imaginary = 21,
  Comma = 22,
  Power = 23,
  Sine = 24,
  Cosine = 25,
  Tangent = 26,
  Pi = 27,
  Sqrt = 28,
  Square = 29,
  Seven = 30,
  Eight = 31,
  Nine = 32,
  Left_parenthesis = 33,
  Right_parenthesis = 34,
  Four = 36,
  Five = 37,
  Six = 38,
  Multiplication = 39,
  Division = 40,
  One  = 42,
  Two = 43,
  Three = 44,
  Plus = 45,
  Minus = 46,
  Zero = 48,
  Dot = 49,
  Ee = 50,
  Ans = 51,
  Exe = 52,
  // Shift
  Shift_left  = 54,
  Shift_up    = 55,
  Shift_down  = 56,
  Shift_right = 57,
  Alpha_lock = 67,
  Cut = 68,
  Copy = 69,
  Paste = 70,
  Clear = 71,
  Left_bracket = 72,
  Right_bracket = 73,
  Left_brace = 74,
  Right_brace = 75,
  Underscore = 76,
  Sto = 77,
  Arcsine = 78,
  Arccosine =   79,
  Arctangent = 80,
  Equal = 81,
  Lower = 82,
  Greater = 83,
  // Alpha
  Colon = 122,
  Semicolon = 123,
  Double_quotes = 124,
  Percent = 125,
  Lower_a = 126,
  Lower_b = 127,
  Lower_c = 128,
  Lower_d = 129,
  Lower_e = 130,
  Lower_f = 131,
  Lower_g = 132,
  Lower_h = 133,
  Lower_i = 134,
  Lower_j = 135,
  Lower_k = 136,
  Lower_l = 137,
  Lower_m = 138,
  Lower_n = 139,
  Lower_o = 140,
  Lower_p = 141,
  Lower_q = 142,
  Lower_r = 144,
  Lower_s = 145,
  Lower_t = 146,
  Lower_u = 147,
  Lower_v = 148,
  Lower_w = 150,
  Lower_x = 151,
  Lower_y = 152,
  Lower_z = 153,
  Space = 154,
  Question = 156,
  Exclamation = 157,
  // Shift alpha
  Upper_a = 180,
  Upper_b = 181,
  Upper_c = 182,
  Upper_d = 183,
  Upper_e = 184,
  Upper_f = 185,
  Upper_g = 186,
  Upper_h = 187,
  Upper_i = 188,
  Upper_j = 189,
  Upper_k = 190,
  Upper_l = 191,
  Upper_m = 192,
  Upper_n = 193,
  Upper_o = 194,
  Upper_p = 195,
  Upper_q = 196,
  Upper_r = 198,
  Upper_s = 199,
  Upper_t = 200,
  Upper_u = 201,
  Upper_v = 202,
  Upper_w = 204,
  Upper_x = 205,
  Upper_y = 206,
  Upper_z = 207,
  // Special
  None = 216,
  Termination = 217,
  USBEnumeration = 219,
  USBPlug = 220,
  BatteryCharging = 221,
  Idle = 223
};

static const char * EventText[] = {
   // Plain
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    nullptr,
    "",
    nullptr,
    nullptr,
    nullptr,
    "",
    "",
    "",
    "",
    "",
    "",
    "e^()",
    "ln()",
    "log()",
    "i",
    ",",
    "^",
    "sin()",
    "cos()",
    "tan()",
    "π",
    "√()",
    "^2",
    "7",
    "8",
    "9",
    "(",
    ")",
    nullptr,
    "4",
    "5",
    "6",
    "×",
    "/",
    nullptr,
    "1",
    "2",
    "3",
    "+",
    "-",
    nullptr,
    "0",
    ".",
    "ᴇ",
    "",
    "",
    nullptr,
    // Shift
    "",
    "",
    "",
    "",
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    "",
    "",
    "",
    "",
    "[",
    "]",
    "{",
    "}",
    "_",
    "",
    "arcsin()",
    "arccos()",
    "arctan()",
    "=",
    "<",
    ">",
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    // Alpha
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ":",
    ";",
    "\"",
    "%",
    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
    "g",
    "h",
    "i",
    "j",
    "k",
    "l",
    "m",
    "n",
    "o",
    "p",
    "q",
    nullptr,
    "r",
    "s",
    "t",
    "u",
    "v",
    nullptr,
    "w",
    "x",
    "y",
    "z",
    " ",
    nullptr,
    "?",
    "!",
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    // Shift+Alpha
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    nullptr,
    "R",
    "S",
    "T",
    "U",
    "V",
    nullptr,
    "W",
    "X",
    "Y",
    "Z",
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    // Special
    "",
    "",
    nullptr,
    "",
    "",
    "",
    nullptr,
    "",
};

class State {
public:
  constexpr State(uint64_t s = 0) : m_bitField(s) { }
  inline bool keyDown(Key k) const {
     return eadk_keyboard_key_down(*this, (eadk_key_t)k);
    //return (m_bitField>>(uint8_t)k) & 1;
  }
  operator eadk_keyboard_state_t() const { return *reinterpret_cast<const eadk_keyboard_state_t *>(this); }
private:
  uint64_t m_bitField;
};
static_assert(sizeof(EADK::Keyboard::State) == sizeof(eadk_keyboard_state_t), "EADK::Keyboard::State should match eadk_keyboard_state_t");


static inline State scan() {
  return State(eadk_keyboard_scan());
}

static inline Event getEvent(int32_t * timeout) {
  return (Event)eadk_event_get(timeout);
}

static inline const char * getText(Event e) {
  return EventText[(int)e];
}

}

namespace Timing {

static inline void msleep(uint32_t ms) {
  return eadk_timing_msleep(ms);
}

}

static inline uint32_t random() {
  return eadk_random();
}

}

#endif
