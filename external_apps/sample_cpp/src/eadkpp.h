#ifndef EADKPP_H
#define EADKPP_H

extern "C" {
#include <eadk.h>
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
  constexpr operator eadk_point_t() const { return *reinterpret_cast<const eadk_point_t *>(this); }
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
  constexpr operator eadk_rect_t() const { return *reinterpret_cast<const eadk_rect_t *>(this); }
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

class State {
public:
  constexpr State(uint64_t s = 0) : m_bitField(s) { }
  inline bool keyDown(Key k) const {
     return eadk_keyboard_key_down(*this, (eadk_key_t)k);
    //return (m_bitField>>(uint8_t)k) & 1;
  }
  constexpr operator eadk_keyboard_state_t() const { return *reinterpret_cast<const eadk_keyboard_state_t *>(this); }
private:
  uint64_t m_bitField;
};
static_assert(sizeof(EADK::Keyboard::State) == sizeof(eadk_keyboard_state_t), "EADK::Keyboard::State should match eadk_keyboard_state_t");


static inline State scan() {
  return State(eadk_keyboard_scan());
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
