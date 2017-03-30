#ifndef HARDWARE_TEST_KEYBOARD_VIEW_H
#define HARDWARE_TEST_KEYBOARD_VIEW_H

#include <escher.h>

namespace HardwareTest {

class KeyboardView : public View {
public:
  KeyboardView();
  Ion::Keyboard::Key testedKey() const;
  void setDefectiveKey(Ion::Keyboard::Key key);
  void setNextKey();
  void resetTestedKey();
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  void drawKey(int key, KDContext * ctx, KDRect rect) const;
  constexpr static int k_margin = 4;
  constexpr static int k_smallSquareSize = 8;
  constexpr static int k_bigSquareSize = 14;
  constexpr static int k_smallRectHeight = 8;
  constexpr static int k_smallRectWidth = 16;
  constexpr static int k_bigRectHeight = 14;
  constexpr static int k_bigRectWidth = 20;
  Ion::Keyboard::Key m_testedKey;
  int m_defectiveKey[Ion::Keyboard::NumberOfKeys];
};

}

#endif

