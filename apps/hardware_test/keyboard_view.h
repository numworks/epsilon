#ifndef HARDWARE_TEST_KEYBOARD_VIEW_H
#define HARDWARE_TEST_KEYBOARD_VIEW_H

#include <escher.h>

namespace HardwareTest {

class KeyboardView : public View {
public:
  KeyboardView();
  int testedKeyIndex() const;
  void setTestedKeyIndex(int i);
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
  int m_testedKeyIndex;
};
}

#endif

