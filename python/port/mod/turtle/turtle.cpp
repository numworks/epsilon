#include "turtle.h"
#include <escher/palette.h>
#include <kandinsky/ion_context.h>
#include <cmath>
extern "C" {
#include <py/misc.h>
}
#include "../../helpers.h"
#include "../../port.h"

static inline mp_float_t absF(mp_float_t x) { return x >= 0 ? x : -x;}

constexpr static KDCoordinate k_iconSize = 15;
constexpr static KDCoordinate k_iconBodySize = 5;
constexpr static KDCoordinate k_iconHeadSize = 3;
constexpr static KDCoordinate k_iconPawSize = 2;

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
  m_heading = 0;
  m_color = k_defaultColor;
  m_penDown = true;
  m_visible = true;
  m_speed = k_defaultSpeed;
  m_penSize = k_defaultPenSize;
  m_mileage = 0;

  // Draw the turtle
  draw(true);
}

bool Turtle::forward(mp_float_t length) {
  /* cos and sin use radians, we thus need to multiply m_heading by PI/180 to
   * compute the new turtle position. This induces rounding errors that are
   * really visible when one expects a horizontal/vertical line and it is not.
   * We thus make special cases for angles in degrees creating vertical /
   * horizontal lines. */
  if (m_heading == 0) {
    return goTo(m_x + length, m_y);
  }
  if (m_heading == 180 || m_heading == -180) {
    return goTo(m_x - length, m_y);
  }
  if (m_heading == 90 || m_heading == -270) {
    return goTo(m_x, m_y + length);
  }
  if (m_heading == 270 || m_heading == -90) {
    return goTo(m_x, m_y - length);
  }
  return goTo(
    m_x + length * std::cos(m_heading * k_headingScale),
    m_y + length * std::sin(m_heading * k_headingScale)
  );
}

void Turtle::left(mp_float_t angle) {
  setHeading(m_heading + angle);
}

void Turtle::circle(mp_int_t radius, mp_float_t angle) {
  mp_float_t oldHeading = heading();
  mp_float_t length = std::fabs(angle * k_headingScale * radius);
  if (length > 1) {
    for (int i = 1; i < length; i++) {
      mp_float_t progress = i / length;
      // Move the turtle forward
      if (forward(1)) {
        // Keyboard interruption. Return now to let MicroPython process it.
        return;
      }
      setHeadingPrivate(oldHeading+std::copysign(angle*progress, angle*radius));
    }
    forward(1);
    setHeading(oldHeading+angle);
  }
}

bool Turtle::goTo(mp_float_t x, mp_float_t y) {
  mp_float_t oldx = m_x;
  mp_float_t oldy = m_y;
  mp_float_t xLength = absF(std::floor(x) - std::floor(oldx));
  mp_float_t yLength = absF(std::floor(y) - std::floor(oldy));

  enum PrincipalDirection {
    None = 0,
    X = 1,
    Y = 2
  };

  PrincipalDirection principalDirection = xLength > yLength ?
    PrincipalDirection::X :
    (xLength == yLength ?
     PrincipalDirection::None :
     PrincipalDirection::Y);

  mp_float_t length = principalDirection == PrincipalDirection::X ? xLength : yLength;

  if (length > 1) {
    // Tweening function
    for (int i = 1; i < length; i++) {
      mp_float_t progress = i / length;
      erase();
      /* We make sure that each pixel along the principal direction is drawn. If
       * the computation of the position on the principal coordinate is done
       * using a barycenter, roundings might skip some pixels, which results in
       * a dotted line. */
      mp_float_t currentX = xLength == 0 ? x : (principalDirection == PrincipalDirection::Y ? x * progress + oldx * (1 - progress) : oldx + (x > oldx ? i : -i));
      mp_float_t currentY = yLength == 0 ? y : (principalDirection == PrincipalDirection::X ? y * progress + oldy * (1 - progress) : oldy + (y > oldy ? i : -i));
      if (dot(currentX, currentY) || draw(false)) {
        // Keyboard interruption. Return now to let MicroPython process it.
        return true;
      }
    }
  }

  erase();
  dot(x, y);
  draw(true);
  return false;
}

void Turtle::setHeading(mp_float_t angle) {
  micropython_port_vm_hook_loop();
  setHeadingPrivate(angle);
  erase();
  draw(true);
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
    draw(true);
  } else {
    erase();
  }
}

void Turtle::write(const char * string) {
  // We erase the turtle to redraw it on top of the text
  if (isOutOfBounds()) {
    return;
  }
  erase();
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDContext * ctx = KDIonContext::SharedContext();
  constexpr static KDCoordinate headOffsetLength = 6;
  KDCoordinate headOffsetX = headOffsetLength * std::cos(m_heading * k_headingScale);
  KDCoordinate headOffsetY = k_invertedYAxisCoefficient * headOffsetLength * std::sin(m_heading * k_headingScale);
  KDPoint headOffset(headOffsetX, headOffsetY);
  KDPoint head(-k_iconHeadSize, -k_iconHeadSize);
  KDPoint stringOffset = KDPoint(0,-KDFont::GlyphHeight(k_font));
  ctx->drawString(string, position().translatedBy(headOffset).translatedBy(head).translatedBy(stringOffset));
  draw(true);
}


void Turtle::viewDidDisappear() {
  m_drawn = false;
}

bool Turtle::isOutOfBounds() const {
  return absF(x()) > k_maxPosition || absF(y()) > k_maxPosition;
};

// Private functions

void Turtle::setHeadingPrivate(mp_float_t angle) {
  // Put the angle in [0; 360[
  mp_float_t angleLimit = 360;
  mp_float_t angleBetween0And360 = angle - ((angle >= 0 && angle < angleLimit) ? 0 : std::floor(angle/angleLimit) * angleLimit);
  if (angleBetween0And360 >= 0 && angleBetween0And360 < angleLimit) {
    m_heading = angleBetween0And360;
  } else {
    // When angle is too big, our formula does not put it properly in [0; 360[
    m_heading = 0;
  }
}

KDPoint Turtle::position(mp_float_t x, mp_float_t y) const {
  return KDPoint(std::floor(x + k_xOffset), std::floor(k_invertedYAxisCoefficient * y + k_yOffset));
}

bool Turtle::hasUnderneathPixelBuffer() {
  if (m_underneathPixelBuffer != nullptr) {
    return true;
  }
  m_underneathPixelBuffer = allocate<KDColor>(k_iconSize * k_iconSize);
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
  assert(!isOutOfBounds());
  KDPoint iconOffset = KDPoint(-k_iconSize/2, -k_iconSize/2);
  return KDRect(position().translatedBy(iconOffset), k_iconSize, k_iconSize);
}

bool Turtle::draw(bool force) {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();

  if ((m_speed > 0 || force) && m_visible && !m_drawn && hasUnderneathPixelBuffer() && !isOutOfBounds()) {
    KDContext * ctx = KDIonContext::SharedContext();

    // Get the pixels underneath the turtle
    ctx->getPixels(iconRect(), m_underneathPixelBuffer);

    // Draw the body
    KDRect drawingRect = KDRect(
        position().translatedBy(KDPoint(-k_iconBodySize/2, -k_iconBodySize/2)),
        k_iconBodySize,
        k_iconBodySize);
    ctx->fillRect(drawingRect, m_color);

    // Draw the head
    KDCoordinate headOffsetLength = 6;
    KDCoordinate headOffsetX = headOffsetLength * std::cos(m_heading * k_headingScale);
    KDCoordinate headOffsetY = k_invertedYAxisCoefficient * headOffsetLength * std::sin(m_heading * k_headingScale);
    KDPoint headOffset(headOffsetX, headOffsetY);
    drawingRect = KDRect(
        position().translatedBy(headOffset).translatedBy(KDPoint(-k_iconHeadSize/2, -k_iconHeadSize/2)),
        k_iconHeadSize,
        k_iconHeadSize);
    ctx->fillRect(drawingRect, m_color);

    // Draw the paws

    static int j = 0;
    int mod = 12;
    j = (j+1)%mod;

    /*  Our turtle walk:
     *
     *    .  .     .  .     .  .     .      .
     *   ====      ====      ====      ====
     *   ==== O    ==== O    ==== O    ==== O
     *   ====      ====      ====      ====
     *  °  °     °      °     °  °     °  °
     * */

    if (j < mod/4) {
      // First walking position
      drawPaw(PawType::FrontRight, PawPosition::HalfBackwards);
      drawPaw(PawType::BackRight, PawPosition::HalfBackwards);
      drawPaw(PawType::FrontLeft, PawPosition::HalfForward);
      drawPaw(PawType::BackLeft, PawPosition::HalfForward);
    } else if (j < mod/2) {
      // Second walking position
      drawPaw(PawType::FrontRight, PawPosition::Forward);
      drawPaw(PawType::BackRight, PawPosition::Backwards);
      drawPaw(PawType::FrontLeft, PawPosition::Normal);
      drawPaw(PawType::BackLeft, PawPosition::Normal);
    } else if (j < 3*mod/4) {
      // Third walking position
      drawPaw(PawType::FrontRight, PawPosition::HalfForward);
      drawPaw(PawType::BackRight, PawPosition::HalfForward);
      drawPaw(PawType::FrontLeft, PawPosition::HalfBackwards);
      drawPaw(PawType::BackLeft, PawPosition::HalfBackwards);
    } else {
      // Fourth walking position
      drawPaw(PawType::FrontRight, PawPosition::Normal);
      drawPaw(PawType::BackRight, PawPosition::Normal);
      drawPaw(PawType::FrontLeft, PawPosition::Forward);
      drawPaw(PawType::BackLeft, PawPosition::Backwards);
    }
    m_drawn = true;
  }

  /* TODO: Maybe this threshold should be in time (mileage/speed) instead of
   * mileage to interrupt with the same frequency whatever the speed is. */
  if (m_mileage > k_mileageLimit) {
    if (micropython_port_interruptible_msleep(1 + (m_speed == 0 ? 0 : 3 * (k_maxSpeed - m_speed)))) {
      return true;
    }
    m_mileage -= k_mileageLimit;
  }
  return false;
}

bool Turtle::dot(mp_float_t x, mp_float_t y) {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();

  // Draw the dot if the pen is down
  if (m_penDown && hasDotBuffers() && !isOutOfBounds()) {
    KDContext * ctx = KDIonContext::SharedContext();
    KDRect rect(
      position(x, y).translatedBy(KDPoint(-m_penSize/2, -m_penSize/2)),
      KDSize(m_penSize, m_penSize)
    );
    ctx->blendRectWithMask(rect, m_color, m_dotMask, m_dotWorkingPixelBuffer);
  }

  /* Increase the turtle's mileage. We need to make sure the mileage is not
   * overflowed, otherwise we might skip some msleeps in draw. */
  uint16_t additionalMileage = sqrt((x - m_x) * (x - m_x) + (y - m_y) * (y - m_y)) * 1000;
  m_mileage = ((m_mileage > k_mileageLimit)
      && ((m_mileage + additionalMileage) < k_mileageLimit)) ?
    k_mileageLimit + 1 : m_mileage + additionalMileage;

  m_x = x;
  m_y = y;

  return micropython_port_vm_hook_loop();
}

void Turtle::drawPaw(PawType type, PawPosition pos) {
  assert(!m_drawn);
  assert(m_underneathPixelBuffer != nullptr);
  assert(!isOutOfBounds());
  KDCoordinate pawOffset = 5;
  constexpr float crawlOffset = 0.6f;
  constexpr float angles[] = {M_PI_4, 3*M_PI_4, -3*M_PI_4, -M_PI_4};

  // Compute the paw offset from the turtle center
  float currentAngle = angles[(int) type];
  float crawlDelta = ((float)((int)pos)) * crawlOffset;
  float pawX = pawOffset * std::cos(m_heading * k_headingScale + currentAngle) + crawlDelta * std::cos(m_heading * k_headingScale);
  float pawY = k_invertedYAxisCoefficient * (pawOffset * std::sin(m_heading * k_headingScale + currentAngle) + crawlDelta * std::sin(m_heading * k_headingScale));
  KDCoordinate pawOffsetX = ((int)pawX) - (pawX < 0 ? 1 : 0);
  KDCoordinate pawOffsetY = ((int)pawY) - (pawY < 0 ? 1 : 0);
  KDPoint offset(pawOffsetX, pawOffsetY);

  // Draw the paw
  KDRect drawingRect = KDRect(
      position().translatedBy(offset), // The paw is too small to need to offset it from its center
      k_iconPawSize,
      k_iconPawSize);
  KDIonContext::SharedContext()->fillRect(drawingRect, m_color);
}

void Turtle::erase() {
  if (!m_drawn || m_underneathPixelBuffer == nullptr || isOutOfBounds()) {
    return;
  }
  KDContext * ctx = KDIonContext::SharedContext();
  ctx->fillRectWithPixels(iconRect(), m_underneathPixelBuffer, nullptr);
  m_drawn = false;
}
