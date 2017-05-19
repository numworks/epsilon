#ifndef HARDWARE_TEST_KEYBOARD_VIEW_H
#define HARDWARE_TEST_KEYBOARD_VIEW_H

#include <escher.h>

namespace HardwareTest {

class KeyboardView : public View {
public:
  KeyboardView();
  uint8_t testedKey() const;
  void setDefectiveKey(uint8_t key);
  bool setNextKey();
  void resetTestedKey();
  void updateLEDState(KDColor color);
  void updateBatteryState(float batteryLevel, bool batteryCharging);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  void drawKey(uint8_t key, KDContext * ctx, KDRect rect) const;
  void layoutSubviews() override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  constexpr static int k_margin = 4;
  constexpr static int k_smallSquareSize = 8;
  constexpr static int k_bigSquareSize = 14;
  constexpr static int k_smallRectHeight = 8;
  constexpr static int k_smallRectWidth = 16;
  constexpr static int k_bigRectHeight = 14;
  constexpr static int k_bigRectWidth = 20;
  constexpr static int k_maxNumberOfCharacters = 20;
  uint8_t m_testedKey;
  bool m_defectiveKey[Ion::Keyboard::NumberOfValidKeys];
  BufferTextView m_batteryLevelView;
  BufferTextView m_batteryChargingView;
  BufferTextView m_ledStateView;
};

}

#endif

