#include "code_128b_view.h"

#include <string.h>

namespace HardwareTest {

constexpr uint16_t patterns[] = {
    0x6CC, 0x66C, 0x666, 0x498, 0x48C, 0x44C, 0x4C8, 0x4C4, 0x464, 0x648, 0x644,
    0x624, 0x59C, 0x4DC, 0x4CE, 0x5CC, 0x4EC, 0x4E6, 0x672, 0x65C, 0x64E, 0x6E4,
    0x674, 0x76E, 0x74C, 0x72C, 0x726, 0x764, 0x734, 0x732, 0x6D8, 0x6C6, 0x636,
    0x518, 0x458, 0x446, 0x588, 0x468, 0x462, 0x688, 0x628, 0x622, 0x5B8, 0x58E,
    0x46E, 0x5D8, 0x5C6, 0x476, 0x776, 0x68E, 0x62E, 0x6E8, 0x6E2, 0x6EE, 0x758,
    0x746, 0x716, 0x768, 0x762, 0x71A, 0x77A, 0x642, 0x78A, 0x530, 0x50C, 0x4B0,
    0x486, 0x42C, 0x426, 0x590, 0x584, 0x4D0, 0x4C2, 0x434, 0x432, 0x612, 0x650,
    0x7BA, 0x614, 0x47A, 0x53C, 0x4BC, 0x49E, 0x5E4, 0x4F4, 0x4F2, 0x7A4, 0x794,
    0x792, 0x6DE, 0x6F6, 0x7B6, 0x578, 0x51E, 0x45E, 0x5E8, 0x5E2, 0x7A8, 0x7A2,
    0x5DE, 0x5EE, 0x75E, 0x7AE};

constexpr uint16_t startPattern = 0x690;
constexpr uint16_t stopPattern = 0x18EB;
constexpr KDColor Code128BView::k_borderColor;

Code128BView::Code128BView() : View(), m_moduleWidth(0), m_data{nullptr} {}

void Code128BView::setData(const char* data) {
  m_data = data;
  markRectAsDirty(bounds());
}

void Code128BView::layoutSubviews(bool force) { updateModuleWidth(); }

void Code128BView::updateModuleWidth() {
  if (m_data == nullptr) {
    m_moduleWidth = 0;
    return;
  }

  int numberOfModules = 10 +                   // quiet zone
                        11 +                   // start
                        11 * strlen(m_data) +  // data
                        11 +                   // checksum
                        13;                    // stop

  m_moduleWidth = bounds().width() / numberOfModules;
}

void Code128BView::drawQuietZoneAt(KDContext* ctx, KDCoordinate* x) const {
  ctx->fillRect(KDRect(*x, 0, 10 * m_moduleWidth, bounds().height()),
                KDColorWhite);
  *x = *x + 10 * m_moduleWidth;
}

void Code128BView::drawPatternAt(KDContext* ctx, uint16_t pattern,
                                 KDCoordinate* x, KDCoordinate width) const {
  assert(width <= 16);
  for (int i = 0; i < width; i++) {
    if (pattern & (1 << (width - i - 1))) {
      ctx->fillRect(KDRect(*x, 0, m_moduleWidth, bounds().height()),
                    KDColorBlack);
    }
    *x = *x + m_moduleWidth;
  }
}

void Code128BView::drawCharAt(KDContext* ctx, char c, KDCoordinate* x) const {
  return drawPatternAt(ctx, patterns[c - 32], x);
}

int Code128BView::checksum() const {
  int checksum = 104;
  const char* letter = m_data;
  int i = 1;
  while (*letter != 0) {
    checksum += (*letter - 32) * i;
    i++;
    letter++;
  }
  return checksum % 103;
}

// Serial Number is 96 bits = 4 * 24 bits = 4 * 4 letters in Base64 = 16 letters

void Code128BView::drawRect(KDContext* ctx, KDRect rect) const {
  if (m_moduleWidth == 0) {
    // Not enough space for given string
    ctx->fillRect(rect, KDColorRed);
    return;
  }

  ctx->fillRect(rect, KDColorWhite);

  KDCoordinate x = 0;
  drawQuietZoneAt(ctx, &x);
  drawPatternAt(ctx, startPattern, &x);
  const char* letter = m_data;
  while (*letter != 0) {
    drawCharAt(ctx, *letter, &x);
    letter++;
  }
  drawPatternAt(ctx, patterns[checksum()], &x);
  drawPatternAt(ctx, stopPattern, &x, 13);
  drawQuietZoneAt(ctx, &x);

  ctx->drawString(m_data, KDPoint(k_stringOffset, k_stringOffset));

  // Draw a red border to test the screen centered position
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_outlineThickness),
                k_borderColor);
  ctx->fillRect(KDRect(bounds().width() - k_outlineThickness, 0,
                       k_outlineThickness, bounds().height()),
                k_borderColor);
  ctx->fillRect(KDRect(0, bounds().height() - k_outlineThickness,
                       bounds().width(), k_outlineThickness),
                k_borderColor);
  ctx->fillRect(KDRect(0, 0, k_outlineThickness, bounds().height()),
                k_borderColor);

  KDColor enhanceColor = KDColorBlack;
  ctx->fillRect(
      KDRect(k_outlineThickness, k_outlineThickness,
             bounds().width() - 2 * k_outlineThickness, 2 * k_outlineThickness),
      enhanceColor);
  ctx->fillRect(KDRect(bounds().width() - 3 * k_outlineThickness,
                       k_outlineThickness, 2 * k_outlineThickness,
                       bounds().height() - 2 * k_outlineThickness),
                enhanceColor);
  ctx->fillRect(
      KDRect(k_outlineThickness, bounds().height() - 3 * k_outlineThickness,
             bounds().width() - 2 * k_outlineThickness, 2 * k_outlineThickness),
      enhanceColor);
  ctx->fillRect(
      KDRect(k_outlineThickness, k_outlineThickness, 2 * k_outlineThickness,
             bounds().height() - 2 * k_outlineThickness),
      enhanceColor);
}

}  // namespace HardwareTest
