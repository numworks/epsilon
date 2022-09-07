#ifndef PYTHON_PORT_MOD_TURTLE_TURTLE_H
#define PYTHON_PORT_MOD_TURTLE_TURTLE_H

extern "C" {
#include <py/mpconfig.h>
}
#include <ion.h>
#include <escher/metric.h>
#include <math.h>
#include <python/port/port.h>

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
    m_underneathPixelBuffer(nullptr),
    m_dotMask(nullptr),
    m_dotWorkingPixelBuffer(nullptr),
    m_x(0),
    m_y(0),
    m_heading(0),
    m_color(k_defaultColor),
    m_colorMode(MicroPython::Color::Mode::MaxIntensity255),
    m_penDown(true),
    m_visible(true),
    m_speed(k_defaultSpeed),
    m_penSize(k_defaultPenSize),
    m_mileage(0),
    m_drawn(false)
  {
  }

  void reset();

  bool forward(mp_float_t length);
  void backward(mp_float_t length) { forward(-length); }
  void right(mp_float_t angle) { left(-angle); }
  void left(mp_float_t angle);
  void circle(mp_int_t radius, mp_float_t angle = 360);
  bool goTo(mp_float_t x, mp_float_t y);

  mp_float_t heading() const { return m_heading; }
  void setHeading(mp_float_t angle);

  uint8_t speed() const { return m_speed; }
  void setSpeed(mp_int_t speed);

  mp_float_t x() const { return m_x; }
  mp_float_t y() const { return m_y; }

  KDCoordinate penSize() const { return m_penSize; }
  void setPenSize(KDCoordinate penSize);
  bool isPenDown() const { return m_penDown; }
  void setPenDown(bool penDown) { m_penDown = penDown; }

  bool isVisible() const { return m_visible; }
  void setVisible(bool visible);

  KDColor color() const { return m_color; }
  void setColor(KDColor c) {
    m_color = c;
  }
  void setColor(uint8_t r, uint8_t g, uint8_t b) {
    m_color = KDColor::RGB888(r, g, b);
  }
  MicroPython::Color::Mode colorMode() const {return m_colorMode; }
  void setColorMode(MicroPython::Color::Mode colorMode){
    m_colorMode = colorMode;
  }

  void write(const char * string);

  void viewDidDisappear();

  /* isOutOfBounds returns true if nothing should be drawn at current position.
   * We avoid drawing at extreme position (far from screen bounds) to prevent
   * coordinate overflows. However, this solution makes the turtle go faster
   * when out of bound, and can prevent text that would have been visible to be
   * drawn. We use very large bounds to temper these effects. */
  bool isOutOfBounds() const;

private:
  constexpr static mp_float_t k_headingScale = M_PI / 180;
  /* The Y axis is oriented upwards in Turtle and downwards in Kandinsky, so we
   * need to invert some values, hence k_invertedYAxisCoefficient. */
  constexpr static int k_invertedYAxisCoefficient = -1;
  constexpr static KDCoordinate k_xOffset = Ion::Display::Width / 2;
  constexpr static KDCoordinate k_yOffset = Escher::Metric::DisplayHeightWithoutTitleBar / 2;
  constexpr static uint8_t k_defaultSpeed = 8;
  constexpr static uint8_t k_maxSpeed = 10;
  constexpr static KDColor k_defaultColor = KDColorBlack;
  constexpr static uint8_t k_defaultPenSize = 1;
  constexpr static KDFont::Size k_font = KDFont::Size::Large;
  constexpr static mp_float_t k_maxPosition = KDCOORDINATE_MAX * 0.75f;

  enum class PawType : uint8_t {
    FrontRight = 0,
    BackRight = 1,
    FrontLeft = 2,
    BackLeft = 3
  };

  enum class PawPosition : int8_t {
    Backwards = -1,
    HalfBackwards = -2,
    Normal = 0,
    HalfForward = 1,
    Forward = 2
  };

  void setHeadingPrivate(mp_float_t angle);
  KDPoint position(mp_float_t x, mp_float_t y) const;
  KDPoint position() const { return position(m_x, m_y); }

  bool hasUnderneathPixelBuffer();
  bool hasDotMask();
  bool hasDotBuffers();

  KDRect iconRect() const;

  // Interruptible methods that return true if they have been interrupted
  bool draw(bool force);
  bool dot(mp_float_t x, mp_float_t y);

  void drawPaw(PawType type, PawPosition position);
  void erase();

  /* When GC is performed, sTurtle is marked as root for GC collection and its
   * data is scanned for pointers that point to the Python heap. We put the 3
   * pointers that should be marked at the beginning of the object to maximize
   * the chances they will be correctly aligned and interpreted as pointers. */
  KDColor * m_underneathPixelBuffer;
  uint8_t * m_dotMask;
  KDColor * m_dotWorkingPixelBuffer;

  /* The frame's center is the center of the screen, the x axis goes to the
   * right and the y axis goes upwards. */
  mp_float_t m_x;
  mp_float_t m_y;
  /* The heading is the angle in degrees between the direction of the turtle and
   * the X axis, in the counterclockwise direction. */
  mp_float_t m_heading;

  KDColor m_color;
  MicroPython::Color::Mode m_colorMode;
  bool m_penDown;
  bool m_visible;

  uint8_t m_speed; // Speed is between 0 and 10
  KDCoordinate m_penSize;

  /* We sleep every time the turtle walks a mileageLimit amount, to allow user
   * interruptions. The length of each sleep is determined by the speed of the
   * turtle.
   * With emscripten, sleep gives control to the web browser, which decides when
   * to return from sleep: this makes the turtle significantly slower on the web
   * emulator than on the calculator. We thus decided to sleep less often on the
   * emscripten platform. */
#if __EMSCRIPTEN__
  constexpr static uint16_t k_mileageLimit = 10000;
#else
  constexpr static uint16_t k_mileageLimit = 1000;
#endif

  uint16_t m_mileage;
  bool m_drawn;

};

#endif
