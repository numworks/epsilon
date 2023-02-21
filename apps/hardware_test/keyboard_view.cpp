#include "keyboard_view.h"

#include "../constant.h"

namespace HardwareTest {

void KeyboardView::setTestedKeyIndex(int i) {
  m_keyboardModel.setTestedKeyIndex(i);
  markRectAsDirty(bounds());
}

void KeyboardView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
  for (int i = 0; i < Ion::Keyboard::NumberOfValidKeys; i++) {
    drawKey(i, ctx, rect);
  }
}

void KeyboardView::drawKey(int keyIndex, KDContext* ctx, KDRect rect) const {
  Ion::Keyboard::Key key = Ion::Keyboard::ValidKeys[keyIndex];
  KDColor color = keyColor(key);
  /* the key is on the cross */
  if ((uint8_t)key < 4) {
    KDCoordinate x = (uint8_t)key == 1 || (uint8_t)key == 2
                         ? k_margin + k_smallSquareSize
                         : k_margin;
    x = (uint8_t)key == 3 ? x + 2 * k_smallSquareSize : x;
    KDCoordinate y = (uint8_t)key == 0 || (uint8_t)key == 3
                         ? k_margin + k_smallSquareSize
                         : k_margin;
    y = (uint8_t)key == 2 ? y + 2 * k_smallSquareSize : y;
    ctx->fillRect(KDRect(x, y, k_smallSquareSize, k_smallSquareSize), color);
  }
  /* the key is a "OK" or "back" */
  if ((uint8_t)key >= 4 && (uint8_t)key < 6) {
    KDCoordinate x =
        (uint8_t)key == 4
            ? 5 * k_margin + 3 * k_smallSquareSize + 2 * k_bigRectWidth
            : 6 * k_margin + 3 * k_smallSquareSize + 2 * k_bigRectWidth +
                  k_bigSquareSize;
    KDCoordinate y = 2 * k_margin;
    ctx->fillRect(KDRect(x, y, k_bigSquareSize, k_bigSquareSize), color);
  }
  /* the key is a "home" or "power" */
  if ((uint8_t)key >= 6 && (uint8_t)key <= 8) {
    KDCoordinate x = 3 * k_margin + 3 * k_smallSquareSize;
    KDCoordinate y =
        (uint8_t)key == 6 ? k_margin : 2 * k_margin + k_bigRectHeight;
    ctx->fillRect(KDRect(x, y, k_bigRectWidth, k_bigRectHeight), color);
  }
  /* the key is a small key as "shift", "alpha" ...*/
  if ((uint8_t)key >= 12 && (uint8_t)key < 30) {
    int j = ((uint8_t)key - 12) / 6;
    int i = ((uint8_t)key - 12) - 6 * j;
    KDCoordinate x = (i + 1) * k_margin + i * k_smallRectWidth;
    KDCoordinate y =
        2 * k_bigRectHeight + (j + 3) * k_margin + j * k_smallRectHeight;
    ctx->fillRect(KDRect(x, y, k_smallRectWidth, k_smallRectHeight), color);
  }
  /* the key is a big key as "7", "Ans" ...*/
  if ((uint8_t)key >= 30 && (uint8_t)key != 35 && (uint8_t)key != 41 &&
      (uint8_t)key != 47 && (uint8_t)key != 53) {
    int j = ((uint8_t)key - 30) / 6;
    int i = ((uint8_t)key - 30) - 6 * j;
    KDCoordinate x = (i + 1) * k_margin + i * k_bigRectWidth;
    KDCoordinate y = 2 * k_bigRectHeight + 3 * k_smallRectHeight +
                     (j + 6) * k_margin + j * k_bigRectHeight;
    ctx->fillRect(KDRect(x, y, k_bigRectWidth, k_bigRectHeight), color);
  }
}

KDColor KeyboardView::keyColor(Ion::Keyboard::Key key) const {
  if (!m_keyboardModel.belongsToTestedKeysSubset(key)) {
    return Escher::Palette::GrayBright;
  }
  if (m_keyboardModel.testedKey() == key) {
    return KDColorBlue;
  }
  return (uint8_t)key < (uint8_t)m_keyboardModel.testedKey() ? KDColorGreen
                                                             : KDColorBlack;
}

}  // namespace HardwareTest
