#include "keyboard_view.h"
#include "../constant.h"
#include <poincare.h>

using namespace Poincare;

namespace HardwareTest {

KeyboardView::KeyboardView() :
  m_testedKey(Ion::Keyboard::Key::A1),
  m_batteryLevelView(BufferTextView(KDText::FontSize::Small)),
  m_batteryChargingView(BufferTextView(KDText::FontSize::Small)),
  m_ledStateView(BufferTextView(KDText::FontSize::Small))
{
  for (int i = 0; i < Ion::Keyboard::NumberOfKeys; i++) {
    m_defectiveKey[i] = 0;
  }
}

Ion::Keyboard::Key KeyboardView::testedKey() const {
  return m_testedKey;
}

void KeyboardView::setDefectiveKey(Ion::Keyboard::Key key) {
  int keyIndex = (int)key;
  if (keyIndex < Ion::Keyboard::NumberOfKeys && keyIndex >= 0) {
    m_defectiveKey[(int)key] = 1;
  }
}

bool KeyboardView::setNextKey() {
  m_testedKey = (Ion::Keyboard::Key)((int)m_testedKey+1);
  int keyIndex = (int)m_testedKey;
  if (keyIndex == 55) {
    return false;
  }
  if ((keyIndex > 7 && keyIndex < 12) || keyIndex == 35 || keyIndex == 41 || keyIndex == 47 || keyIndex == 53) {
    setNextKey();
  }
  markRectAsDirty(bounds());
  return true;
}

void KeyboardView::resetTestedKey() {
  for (int i = 0; i < Ion::Keyboard::NumberOfKeys; i++) {
    m_defectiveKey[i] = 0;
  }
  m_testedKey = Ion::Keyboard::Key::A1;
  markRectAsDirty(bounds());
}

void KeyboardView::updateLEDState(KDColor color) {
  Ion::LED::setColor(color);

  char ledLevel[k_maxNumberOfCharacters];
  const char * legend = "LED color: ";
  int legendLength = strlen(legend);
  int numberOfChar = legendLength;
  strlcpy(ledLevel, legend, legendLength+1);
  legend = "Off";
  if (color == KDColorWhite) {
    legend = "White";
  }
  if (color == KDColorRed) {
    legend = "Red";
  }
  if (color == KDColorBlue) {
    legend = "Blue";
  }
  if (color == KDColorGreen) {
    legend = "Green";
  }
  legendLength = strlen(legend);
  strlcpy(ledLevel+numberOfChar, legend, legendLength+1);
  m_ledStateView.setText(ledLevel);

  markRectAsDirty(bounds());
}

void KeyboardView::updateBatteryState(float batteryLevel, bool batteryCharging) {
  char bufferLevel[k_maxNumberOfCharacters + Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char * legend = "Battery level: ";
  int legendLength = strlen(legend);
  strlcpy(bufferLevel, legend, legendLength+1);
  Complex::convertFloatToText(batteryLevel, bufferLevel+legendLength, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  m_batteryLevelView.setText(bufferLevel);

  char bufferCharging[k_maxNumberOfCharacters + Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  int numberOfChars = 0;
  legend = "Battery charging: ";
  legendLength = strlen(legend);
  strlcpy(bufferCharging, legend, legendLength+1);
  numberOfChars += legendLength;
  legend = "no";
  if (batteryCharging) {
    legend = "yes";
  }
  legendLength = strlen(legend);
  strlcpy(bufferCharging+numberOfChars, legend, legendLength+1);
  numberOfChars += legendLength;
  bufferCharging[numberOfChars] = 0;
  m_batteryChargingView.setText(bufferCharging);

  markRectAsDirty(bounds());
}

void KeyboardView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
  for (int i = 0; i < Ion::Keyboard::NumberOfKeys; i++) {
     drawKey(i, ctx, rect);
  }
}

void KeyboardView::drawKey(int keyIndex, KDContext * ctx, KDRect rect) const {
  KDColor color = keyIndex == (int)m_testedKey ? KDColorBlue: KDColorBlack;
  if (keyIndex < (int)m_testedKey) {
    color = m_defectiveKey[keyIndex] == 0 ? KDColorGreen : KDColorRed;
  }
  /* the key is on the cross */
  if (keyIndex < 4) {
    KDCoordinate x = keyIndex == 1 || keyIndex == 2 ? k_margin + k_smallSquareSize : k_margin;
    x = keyIndex == 3 ? x + 2*k_smallSquareSize : x;
    KDCoordinate y = keyIndex == 0 || keyIndex == 3 ? k_margin + k_smallSquareSize : k_margin;
    y = keyIndex == 2 ? y + 2*k_smallSquareSize : y;
    ctx->fillRect(KDRect(x, y, k_smallSquareSize, k_smallSquareSize), color);
  }
  /* the key is a "OK" or "back" */
  if (keyIndex >= 4 && keyIndex < 6) {
    KDCoordinate x = keyIndex == 4 ? 5*k_margin + 3*k_smallSquareSize + 2*k_bigRectWidth : 6*k_margin + 3*k_smallSquareSize + 2*k_bigRectWidth + k_bigSquareSize;
    KDCoordinate y = 2*k_margin;
    ctx->fillRect(KDRect(x, y, k_bigSquareSize, k_bigSquareSize), color);
  }
  /* the key is a "home" or "power" */
  if (keyIndex >= 6 && keyIndex < 8) {
    KDCoordinate x = 3*k_margin + 3*k_smallSquareSize;
    KDCoordinate y = keyIndex == 6 ? k_margin : 2*k_margin + k_bigRectHeight;
    ctx->fillRect(KDRect(x, y, k_bigRectWidth, k_bigRectHeight), color);
  }
  /* the key is a small key as "shift", "alpha" ...*/
  if (keyIndex >= 12 && keyIndex < 30) {
    int j = (keyIndex - 12)/6;
    int i = (keyIndex - 12) - 6*j;
    KDCoordinate x = (i+1)*k_margin + i*k_smallRectWidth;
    KDCoordinate y = 2*k_bigRectHeight + (j+3)*k_margin + j*k_smallRectHeight;
    ctx->fillRect(KDRect(x, y, k_smallRectWidth, k_smallRectHeight), color);
  }
  /* the key is a big key as "7", "Ans" ...*/
  if (keyIndex >= 30 && keyIndex != 35 && keyIndex != 41 && keyIndex != 47 && keyIndex !=  53) {
    int j = (keyIndex - 30)/6;
    int i = (keyIndex - 30) - 6*j;
    KDCoordinate x = (i+1)*k_margin + i*k_bigRectWidth;
    KDCoordinate y = 2*k_bigRectHeight + 3*k_smallRectHeight + (j+6)*k_margin + j*k_bigRectHeight;
    ctx->fillRect(KDRect(x, y, k_bigRectWidth, k_bigRectHeight), color);
  }
}

void KeyboardView::layoutSubviews() {
  KDSize textSize = KDText::stringSize(" ", KDText::FontSize::Small);
  m_batteryLevelView.setFrame(KDRect(130, k_margin, bounds().width()-130, textSize.height()));
  m_batteryChargingView.setFrame(KDRect(130, k_margin+2*textSize.height(), bounds().width()-130, textSize.height()));
  m_ledStateView.setFrame(KDRect(130, k_margin+5*textSize.height(), bounds().width()-130, textSize.height()));
}

int KeyboardView::numberOfSubviews() const {
  return 3;
}

View * KeyboardView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_batteryLevelView;
  }
  if (index == 1) {
    return &m_batteryChargingView;
  }
  return &m_ledStateView;
}

}
