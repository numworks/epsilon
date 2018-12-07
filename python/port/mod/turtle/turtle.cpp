#include "turtle.h"
extern "C" {
#include <py/misc.h>
}
#include "../../helpers.h"
#include "../../port.h"
#include "turtle_icon.h"

// TODO add icon in the names
static constexpr KDCoordinate k_turtleSize = 15;
static constexpr KDCoordinate k_turtleBodySize = 5;
static constexpr KDCoordinate k_turtleHeadSize = 3;
static constexpr KDCoordinate k_turtlePawSize = 2;

constexpr KDColor Turtle::k_defaultColor;

template <typename T> static inline T * allocate(size_t count) {
  /* We forward dynamic allocations to the Python GC so we don't have to bother
   * with deallocation. For this to work well, the Turtle object who owns the
   * allocated areas need to be added to MicroPython's GC roots, otherwise those
   * buffers will be wiped out by the GC too early. */
  return static_cast<T*>(m_malloc(sizeof(T) * count));
}

void Turtle::reset() {
  // Erase the drawing
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->resetSandbox();

  // Reset turtle values
  m_x = 0;
  m_y = 0;
  m_heading = k_headingOffset;
  m_color = k_defaultColor;
  m_penDown = true;
  m_visible = true;
  m_speed = k_defaultSpeed;
  m_penSize = k_defaultPenSize;
  m_mileage = 0;

  // Draw the turtle
  draw();
}

bool Turtle::forward(mp_float_t length) {
  return goTo(
    m_x + length * sin(m_heading),
    m_y + length * cos(m_heading)
  );
}

void Turtle::left(mp_float_t angle) {
  setHeading(
     k_invertedYAxisCoefficient * ((m_heading - k_headingOffset) + k_invertedYAxisCoefficient * (angle * k_headingScale)) / k_headingScale
  );
}

void Turtle::circle(mp_int_t radius, mp_float_t angle) {
  mp_float_t oldHeading = heading();
  mp_float_t length = (angle*k_headingScale)*radius;
  if (length > 1) {
    int i = 1;
    while(i <= length) {
      mp_float_t progress = i / length;
      if (m_speed > 0 && m_speed < k_maxSpeed) {
        Ion::Display::waitForVBlank();
      }
      // Move the turtle forward
      if (forward(1)) {
        // Keyboard interruption. Return now to let MicroPython process it.
        return;
      }
      setHeadingPrivate(oldHeading+angle*progress);
      i++;
    }
    forward(length-(i-1));
    setHeading(oldHeading+angle);
  }
}

bool Turtle::goTo(mp_float_t x, mp_float_t y) {
  mp_float_t oldx = m_x;
  mp_float_t oldy = m_y;
  mp_float_t length = sqrt((x - oldx) * (x - oldx) + (y - oldy) * (y - oldy));

  if (length > 1) {
    // Tweening function
    for (int i = 0; i < length; i++) {
      mp_float_t progress = i / length;

      if (m_speed > 0 && m_speed < k_maxSpeed) {
        Ion::Display::waitForVBlank();
      }
      erase();
      if (dot(x * progress + oldx * (1 - progress), y * progress + oldy * (1 - progress))
          || draw())
      {
        // Keyboard interruption. Return now to let MicroPython process it.
        return true;
      }
    }
  }

  Ion::Display::waitForVBlank();
  erase();
  dot(x, y);
  draw();
  return false;
}

mp_float_t Turtle::heading() const {
  return k_invertedYAxisCoefficient * (m_heading - k_headingOffset) / k_headingScale;
}

void Turtle::setHeading(mp_float_t angle) {
  micropython_port_vm_hook_loop();

  setHeadingPrivate(angle);

  Ion::Display::waitForVBlank();
  erase();
  draw();
}

void Turtle::setSpeed(mp_int_t speed) {
  if (speed < 0 || speed > k_maxSpeed) {
    m_speed = 0;
  } else {
    m_speed = speed;
  }
}

void Turtle::setPenSize(KDCoordinate penSize) {
  if (m_penSize == penSize) {
    return;
  }

  if (m_dotMask) {
    m_free(m_dotMask);
    m_dotMask = nullptr;
  }

  if (m_dotWorkingPixelBuffer) {
    m_free(m_dotWorkingPixelBuffer);
    m_dotWorkingPixelBuffer = nullptr;
  }

  m_penSize = penSize;
}

void Turtle::setVisible(bool visible) {
  m_visible = visible;
  if (m_visible) {
    draw();
  } else {
    erase();
  }
}

// Private functions

void Turtle::setHeadingPrivate(mp_float_t angle) {
  m_heading = k_invertedYAxisCoefficient * angle * k_headingScale + k_headingOffset;
}

KDPoint Turtle::position(mp_float_t x, mp_float_t y) const {
  return KDPoint(round(x + k_xOffset), round(k_invertedYAxisCoefficient * y + k_yOffset));
}

bool Turtle::hasUnderneathPixelBuffer() {
  if (m_underneathPixelBuffer != nullptr) {
    return true;
  }
  m_underneathPixelBuffer = allocate<KDColor>(k_turtleSize * k_turtleSize);
  return (m_underneathPixelBuffer != nullptr);
}

bool Turtle::hasDotMask() {
  if (m_dotMask != nullptr) {
    return true;
  }
  m_dotMask = allocate<uint8_t>(m_penSize * m_penSize);
  if (m_dotMask == nullptr) {
    return false;
  }

  mp_float_t middle = m_penSize / 2;
  for (int j = 0; j < m_penSize; j++) {
    for (int i = 0; i < m_penSize; i++) {
      mp_float_t distance = sqrt((j - middle)*(j - middle) + (i - middle)*(i - middle)) / (middle+1);
      int value = distance * distance * 255;
      if (value < 0) {
        value = 0;
      } else if (value > 255) {
        value = 255;
      }
      m_dotMask[j*m_penSize+i] = value;
    }
  }

  return true;
}

bool Turtle::hasDotBuffers() {
  if (m_dotWorkingPixelBuffer == nullptr) {
    m_dotWorkingPixelBuffer = allocate<KDColor>(m_penSize * m_penSize);
  }
  return m_dotWorkingPixelBuffer && hasDotMask();
}

KDRect Turtle::iconRect() const {
  KDPoint iconOffset = KDPoint(-k_turtleSize/2, -k_turtleSize/2);
  return KDRect(position().translatedBy(iconOffset), k_turtleSize, k_turtleSize);
}

bool Turtle::draw() {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();

  if (m_visible && hasUnderneathPixelBuffer()) {
    KDContext * ctx = KDIonContext::sharedContext();
    // Get the pixels underneath the turtle
    ctx->getPixels(iconRect(), m_underneathPixelBuffer);

    // Draw the body
    KDRect drawingRect = KDRect(
        position().translatedBy(KDPoint(-k_turtleBodySize/2, -k_turtleBodySize/2)),
        k_turtleBodySize,
        k_turtleBodySize); // TODO make special method with enum class Paw Head Body Whole
    ctx->fillRect(drawingRect, m_color);

    KDCoordinate membersOffsetLength = 6;

    // Draw the head
    KDCoordinate headOffsetX = membersOffsetLength * sin(m_heading);
    KDCoordinate headOffsetY = -membersOffsetLength * cos(m_heading);
    KDPoint headOffset(headOffsetX, headOffsetY);
    drawingRect = KDRect(
        position().translatedBy(headOffset).translatedBy(KDPoint(-k_turtleHeadSize/2, -k_turtleHeadSize/2)),
        k_turtleHeadSize,
        k_turtleHeadSize); // TODO make special method with enum class Paw Head Body Whole
    ctx->fillRect(drawingRect, m_color);

    // Draw the paws
    membersOffsetLength = 5;
    constexpr int numberOfPaws = 4;
    constexpr float angles[numberOfPaws] = {M_PI_2/2, M_PI_2+M_PI_2/2, M_PI+M_PI_2/2, M_PI+M_PI_2+M_PI_2/2};
    constexpr float anglesEven[numberOfPaws] = {M_PI_2/2, M_PI_2+M_PI_2/2, M_PI+M_PI_2/2, M_PI+M_PI_2+M_PI_2/2};
    for (int i = 0; i < numberOfPaws; i++) {
      float pawX = membersOffsetLength * sin(m_heading+angles[i]);
      float pawY = -membersOffsetLength * cos(m_heading+angles[i]);
      KDCoordinate pawOffsetX = ((int)pawX) - (pawX < 0 ? 1 : 0);
      KDCoordinate pawOffsetY = ((int)pawY) - (pawY < 0 ? 1 : 0);
      KDPoint pawOffset(pawOffsetX, pawOffsetY);
      drawingRect = KDRect(
          position().translatedBy(pawOffset), // The paw is too small to need to offset it
          k_turtlePawSize,
          k_turtlePawSize); // TODO make special method with enum class Paw Head Body Whole
      ctx->fillRect(drawingRect, m_color);
    }

    m_drawn = true;
  }

  if (m_mileage > 1000) {
    if (micropython_port_interruptible_msleep(1 + (m_speed == 0 ? 0 : k_maxSpeed * (k_maxSpeed - m_speed)))) {
      return true;
    }
    m_mileage -= 1000;
  }
  return false;
}

bool Turtle::dot(mp_float_t x, mp_float_t y) {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();

  if (m_penDown && hasDotBuffers()) {
    KDContext * ctx = KDIonContext::sharedContext();
    KDRect rect(
      position(x, y).translatedBy(KDPoint(-m_penSize/2, -m_penSize/2)),
      KDSize(m_penSize, m_penSize)
    );
    ctx->blendRectWithMask(rect, m_color, m_dotMask, m_dotWorkingPixelBuffer);
  }

  m_mileage += sqrt((x - m_x) * (x - m_x) + (y - m_y) * (y - m_y)) * 1000;

  m_x = x;
  m_y = y;

  return micropython_port_vm_hook_loop();
}

void Turtle::erase() {
  if (!m_drawn || m_underneathPixelBuffer == nullptr) {
    return;
  }
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->fillRectWithPixels(iconRect(), m_underneathPixelBuffer, nullptr);
  m_drawn = false;
}
