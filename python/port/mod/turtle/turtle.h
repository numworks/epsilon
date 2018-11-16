#ifndef PYTHON_PORT_MOD_TURTLE_TURTLE_H
#define PYTHON_PORT_MOD_TURTLE_TURTLE_H

extern "C" {
#include <py/mpconfig.h>
}
#include <ion.h>
#include <kandinsky.h>
#include <math.h>

class Turtle {
public:
  constexpr Turtle() :
    m_x(0),
    m_y(0),
    m_heading(k_headingOffset),
    m_color(KDColorBlack),
    m_penDown(true),
    m_visible(true),
    m_speed(6),
    m_penSize(5),
    m_mileage(0),
    m_drawn(false),
    m_underneathPixelBuffer(nullptr),
    m_dotMask(nullptr),
    m_dotWorkingPixelBuffer(nullptr),
    m_iconsPixels(nullptr)
  {}

  void forward(mp_float_t length);
  void backward(mp_float_t length) { forward(-length); }
  void right(mp_float_t angle) { left(-angle); }
  void left(mp_float_t angle);
  void goTo(mp_float_t x, mp_float_t y);

  mp_float_t heading() const;
  void setHeading(mp_float_t angle);

  void setSpeed(mp_int_t speed);

  mp_float_t x() const { return m_x; }
  mp_float_t y() const { return m_y; }

  KDCoordinate penSize() const { return m_penSize; }
  void setPenSize(KDCoordinate penSize);
  bool isPenDown() const { return m_penDown; }
  void setPenDown(bool penDown) { m_penDown = penDown; }

  bool isVisible() const { return m_visible; }
  void setVisible(bool visible);

  void setColor(KDColor c) {
    m_color = c;
  }

private:
  static constexpr mp_float_t k_headingOffset = M_PI_2;
  static constexpr mp_float_t k_headingScale = M_PI / 180;
  static constexpr KDCoordinate k_xOffset = Ion::Display::Width / 2;
  static constexpr KDCoordinate k_yOffset = (Ion::Display::Height - 18) / 2;
  static constexpr KDSize k_iconSize = KDSize(9, 9);
  static constexpr int k_numberOfIcons = 8;

  KDPoint position(mp_float_t x, mp_float_t y) const;
  KDPoint position() const { return position(m_x, m_y); }

  bool hasUnderneathPixelBuffer();
  bool hasDotMask();
  bool hasDotBuffers();

  KDRect iconRect() const {
    KDPoint iconOffset = KDPoint(-k_iconSize.width()/2 + 1, -k_iconSize.height()/2 + 1);
    return KDRect(position().translatedBy(iconOffset), k_iconSize);
  }

  const KDColor * icon();

  void draw();
  void erase();
  void dot(mp_float_t x, mp_float_t y);


  mp_float_t m_x;
  mp_float_t m_y;
  mp_float_t m_heading;

  KDColor m_color;
  bool m_penDown;
  bool m_visible;

  uint8_t m_speed; // Speed is between 1 and 10
  KDCoordinate m_penSize;
  KDCoordinate m_mileage;
  bool m_drawn;

  KDColor * m_underneathPixelBuffer;
  uint8_t * m_dotMask;
  KDColor * m_dotWorkingPixelBuffer;
  KDColor * m_iconsPixels;

  // KDColor m_pixelBuffer[100];
};

#endif
