#ifndef PYTHON_PORT_MOD_TURTLE_TURTLE_H
#define PYTHON_PORT_MOD_TURTLE_TURTLE_H

extern "C" {
#include <py/mpconfig.h>
}
#include <ion.h>
#include <escher/metric.h>
#include <kandinsky.h>
#include <math.h>

/* We check for keyboard interruptions using micropython_port_vm_hook_loop and
 * micropython_port_interruptible_msleep, but even if we catch an interruption,
 * Micropython waits until the end of the top-most C++ function execution to
 * process the interruption.
 * This means that if we are executing a very long function, such as
 * forward(100) with small speed, we cannot interrupt it.
 * To fix this, all methods that might be long to return should return
 * immediately if they find an interruption. */


class Turtle {
public:
  constexpr Turtle() :
    m_x(0),
    m_y(0),
    m_heading(k_headingOffset),
    m_color(k_defaultColor),
    m_penDown(true),
    m_visible(true),
    m_speed(k_defaultSpeed),
    m_penSize(k_defaultPenSize),
    m_mileage(0),
    m_drawn(false),
    m_underneathPixelBuffer(nullptr),
    m_dotMask(nullptr),
    m_dotWorkingPixelBuffer(nullptr)
  {
  }

  void reset();

  bool forward(mp_float_t length);
  void backward(mp_float_t length) { forward(-length); }
  void right(mp_float_t angle) { left(-angle); }
  void left(mp_float_t angle);
  void circle(mp_int_t radius, mp_float_t angle);
  bool goTo(mp_float_t x, mp_float_t y);

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
  /* The Y axis is oriented upwards in Turtle and downwards in Kandinsky, so we
   * need to invert some values, hence k_invertedYAxisCoefficient. */
  static constexpr int k_invertedYAxisCoefficient = -1;
  static constexpr KDCoordinate k_xOffset = Ion::Display::Width / 2;
  static constexpr KDCoordinate k_yOffset = (Ion::Display::Height - Metric::TitleBarHeight) / 2;
  static constexpr uint8_t k_defaultSpeed = 3;
  static constexpr uint8_t k_maxSpeed = 10;
  static constexpr KDColor k_defaultColor = KDColorBlack;
  static constexpr uint8_t k_defaultPenSize = 1;

  enum class PawType : uint8_t {
    FrontRight = 0,
    BackRight = 1,
    FrontLeft = 2,
    BackLeft = 3
  };

  enum class PawPosition : int {
    Backwards = 0,
    HalfBackwards = 1,
    Normal = 2,
    HalfForward = 3,
    Forward = 4
  };

  void setHeadingPrivate(mp_float_t angle);
  KDPoint position(mp_float_t x, mp_float_t y) const;
  KDPoint position() const { return position(m_x, m_y); }

  bool hasUnderneathPixelBuffer();
  bool hasDotMask();
  bool hasDotBuffers();

  KDRect iconRect() const;

  // Interruptible methods that return true if they have been interrupted
  bool draw();
  bool dot(mp_float_t x, mp_float_t y);

  void drawPaw(PawType type, PawPosition position);
  void erase();

  /* The frame's center is the center of the screen, the x axis goes to the
   * right and the y axis goes upwards. */
  mp_float_t m_x;
  mp_float_t m_y;
  /* The heading is the angle in radians between the direction of the turtle and
   * the X axis, in the counterclockwise direction. */
  mp_float_t m_heading;

  KDColor m_color;
  bool m_penDown;
  bool m_visible;

  uint8_t m_speed; // Speed is between 0 and 10
  KDCoordinate m_penSize;
  KDCoordinate m_mileage;
  bool m_drawn;

  KDColor * m_underneathPixelBuffer;
  uint8_t * m_dotMask;
  KDColor * m_dotWorkingPixelBuffer;
};

#endif
