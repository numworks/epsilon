#include "keyboard_view.h"
#include "../constant.h"
#include <poincare.h>

using namespace Poincare;

namespace HardwareTest {

KeyboardView::KeyboardView() :
  m_testedKey(0),
  m_batteryLevelView(KDText::FontSize::Small),
  m_batteryChargingView(KDText::FontSize::Small),
  m_ledStateView(KDText::FontSize::Small)
{
  for (uint8_t i = 0; i < Ion::Keyboard::NumberOfValidKeys; i++) {
    m_defectiveKey[i] = 0;
  }
}

uint8_t KeyboardView::testedKey() const {
  return m_testedKey;
}

void KeyboardView::setDefectiveKey(uint8_t key) {
  if (key < Ion::Keyboard::NumberOfValidKeys && key >= 0) {
    m_defectiveKey[key] = 1;
  }
}

bool KeyboardView::setNextKey() {
  m_testedKey = m_testedKey+1;
  if (m_testedKey == 46) {
    return false;
  }
  markRectAsDirty(bounds());
  return true;
}

void KeyboardView::resetTestedKey() {
  for (uint8_t i = 0; i < Ion::Keyboard::NumberOfValidKeys; i++) {
    m_defectiveKey[i] = 0;
  }
  m_testedKey = 0;
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
  for (uint8_t i = 0; i < Ion::Keyboard::NumberOfValidKeys; i++) {
     drawKey(i, ctx, rect);
  }
}

void KeyboardView::drawKey(uint8_t keyIndex, KDContext * ctx, KDRect rect) const {
  KDColor color = keyIndex == m_testedKey ? KDColorBlue: KDColorBlack;
  if (keyIndex < m_testedKey) {
    color = m_defectiveKey[keyIndex] == 0 ? KDColorGreen : KDColorRed;
  }
  Ion::Keyboard::Key key = Ion::Keyboard::ValidKeys[keyIndex];
  /* the key is on the cross */
  if ((uint8_t)key < 4) {
    KDCoordinate x = (uint8_t)key == 1 || (uint8_t)key == 2 ? k_margin + k_smallSquareSize : k_margin;
    x = (uint8_t)key == 3 ? x + 2*k_smallSquareSize : x;
    KDCoordinate y = (uint8_t)key == 0 || (uint8_t)key == 3 ? k_margin + k_smallSquareSize : k_margin;
    y = (uint8_t)key == 2 ? y + 2*k_smallSquareSize : y;
    ctx->fillRect(KDRect(x, y, k_smallSquareSize, k_smallSquareSize), color);
  }
  /* the key is a "OK" or "back" */
  if ((uint8_t)key >= 4 && (uint8_t)key < 6) {
    KDCoordinate x = (uint8_t)key == 4 ? 5*k_margin + 3*k_smallSquareSize + 2*k_bigRectWidth : 6*k_margin + 3*k_smallSquareSize + 2*k_bigRectWidth + k_bigSquareSize;
    KDCoordinate y = 2*k_margin;
    ctx->fillRect(KDRect(x, y, k_bigSquareSize, k_bigSquareSize), color);
  }
  /* the key is a "home" or "power" */
  if ((uint8_t)key >= 6 && (uint8_t)key < 8) {
    KDCoordinate x = 3*k_margin + 3*k_smallSquareSize;
    KDCoordinate y = (uint8_t)key == 6 ? k_margin : 2*k_margin + k_bigRectHeight;
    ctx->fillRect(KDRect(x, y, k_bigRectWidth, k_bigRectHeight), color);
  }
  /* the key is a small key as "shift", "alpha" ...*/
  if ((uint8_t)key >= 12 && (uint8_t)key < 30) {
    int j = ((uint8_t)key - 12)/6;
    int i = ((uint8_t)key - 12) - 6*j;
    KDCoordinate x = (i+1)*k_margin + i*k_smallRectWidth;
    KDCoordinate y = 2*k_bigRectHeight + (j+3)*k_margin + j*k_smallRectHeight;
    ctx->fillRect(KDRect(x, y, k_smallRectWidth, k_smallRectHeight), color);
  }
  /* the key is a big key as "7", "Ans" ...*/
  if ((uint8_t)key >= 30 && (uint8_t)key != 35 && (uint8_t)key != 41 && (uint8_t)key != 47 && (uint8_t)key !=  53) {
    int j = ((uint8_t)key - 30)/6;
    int i = ((uint8_t)key - 30) - 6*j;
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
