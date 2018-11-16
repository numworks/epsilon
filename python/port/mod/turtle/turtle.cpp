#include "turtle.h"
extern "C" {
#include <py/misc.h>
}
#include "../../helpers.h"
#include "../../port.h"
#include "turtle_icon.h"

static constexpr KDSize k_iconSize = KDSize(9, 9);

template <typename T> static inline T * allocate(size_t count) {
  /* We forward dynamic allocations to the Python GC so we don't have to bother
   * with deallocation. For this to work well, the Turtle object who owns the
   * allocated areas need to be added to MicroPython's GC roots, otherwise those
   * buffers will be wiped out by the GC too early. */
  return static_cast<T*>(m_malloc(sizeof(T) * count));
}

void Turtle::forward(mp_float_t length) {
  goTo(
    m_x + length * sin(m_heading),
    m_y + length * cos(m_heading)
  );
}

void Turtle::left(mp_float_t angle) {
  setHeading(
      ((m_heading - k_headingOffset) + (angle * k_headingScale)) / k_headingScale
  );
}

void Turtle::goTo(mp_float_t x, mp_float_t y) {
  mp_float_t oldx = m_x;
  mp_float_t oldy = m_y;
  mp_float_t length = sqrt((x - oldx) * (x - oldx) + (y - oldy) * (y - oldy));

  if (length > 1) {
    // Tweening function
    for (int i = 0; i < length; i++) {
      mp_float_t progress = i / length;

      if (m_speed > 0) {
        Ion::Display::waitForVBlank();
      }
      erase();
      dot(x * progress + oldx * (1 - progress), y * progress + oldy * (1 - progress));
      draw();
    }
  }

  Ion::Display::waitForVBlank();
  erase();
  dot(x, y);
  draw();
}

mp_float_t Turtle::heading() const {
  return (m_heading - k_headingOffset) / k_headingScale;
}

void Turtle::setHeading(mp_float_t angle) {
  micropython_port_vm_hook_loop();

  m_heading = angle * k_headingScale + k_headingScale;

  Ion::Display::waitForVBlank();
  erase();
  draw();
}

void Turtle::setSpeed(mp_int_t speed) {
  // Speed is clamped between 0 and 10
  if (speed < 0) {
    m_speed = 0;
  } else if (speed > 10) {
    m_speed = 10;
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

KDPoint Turtle::position(mp_float_t x, mp_float_t y) const {
  return KDPoint(round(x + k_xOffset), round(y + k_yOffset));
}

bool Turtle::hasUnderneathPixelBuffer() {
  if (m_underneathPixelBuffer != nullptr) {
    return true;
  }
  m_underneathPixelBuffer = allocate<KDColor>(k_iconSize.width() * k_iconSize.height());
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
  KDPoint iconOffset = KDPoint(-k_iconSize.width()/2 + 1, -k_iconSize.height()/2 + 1);
  return KDRect(position().translatedBy(iconOffset), k_iconSize);
}

const KDColor * Turtle::icon() {
  if (m_iconsPixels == nullptr) {
    m_iconsPixels = allocate<KDColor>(k_iconSize.width() * k_iconSize.height() * k_numberOfIcons);
    if (m_iconsPixels == nullptr) {
      return nullptr;
    }

    Ion::decompress(
      ImageStore::TurtleIcon->compressedPixelData(),
      reinterpret_cast<uint8_t *>(m_iconsPixels),
      ImageStore::TurtleIcon->compressedPixelDataSize(),
      sizeof(KDColor) * k_iconSize.width() * k_iconSize.height() * k_numberOfIcons
    );
  }

  int frame = ((m_heading / (2*M_PI)) * k_numberOfIcons + 0.5);
  if (frame < 0) {
    frame = k_numberOfIcons - ((-frame) % k_numberOfIcons) - 1;
  } else {
    frame = frame % k_numberOfIcons;
  }
  int offset = frame * k_iconSize.width() * k_iconSize.height();

  return &m_iconsPixels[offset];
}

void Turtle::draw() {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();

  const KDColor * i = icon();

  if (m_visible && i && hasUnderneathPixelBuffer()) {
    KDContext * ctx = KDIonContext::sharedContext();
    KDRect rect = iconRect();
    ctx->getPixels(rect, m_underneathPixelBuffer);
    ctx->fillRectWithPixels(rect, i, nullptr);
    m_drawn = true;
  }

  if (m_mileage > 1000) {
    if (m_speed > 0) {
      micropython_port_interruptible_msleep(8 * (8 - m_speed));
      m_mileage -= 1000;
    } else {
      m_mileage = 0;
    }
  }
}

void Turtle::erase() {
  if (!m_drawn || m_underneathPixelBuffer == nullptr) {
    return;
  }
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->fillRectWithPixels(iconRect(), m_underneathPixelBuffer, nullptr);
  m_drawn = false;
}

void Turtle::dot(mp_float_t x, mp_float_t y) {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();

  if (m_penDown && hasDotBuffers()) {
    KDContext * ctx = KDIonContext::sharedContext();
    KDRect rect(
      position(x, y).translatedBy(KDPoint(-m_penSize/2, -m_penSize/2)),
      KDSize(m_penSize, m_penSize)
    );
    ctx->blendRectWithMask(rect, m_color, m_dotMask, m_dotWorkingPixelBuffer);
  }

  if (m_speed > 0) {
    m_mileage += sqrt((x - m_x) * (x - m_x) + (y - m_y) * (y - m_y)) * 1000;
  }

  micropython_port_vm_hook_loop();

  m_x = x;
  m_y = y;
}
