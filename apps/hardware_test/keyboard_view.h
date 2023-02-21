#ifndef HARDWARE_TEST_KEYBOARD_VIEW_H
#define HARDWARE_TEST_KEYBOARD_VIEW_H

#include <escher/view.h>

#include "keyboard_model.h"

namespace HardwareTest {

class KeyboardView : public Escher::View {
 public:
  using Escher::View::View;
  int testedKeyIndex() const { return m_keyboardModel.testedKeyIndex(); }
  void setTestedKeyIndex(int i);
  void drawRect(KDContext* ctx, KDRect rect) const override;

 private:
  void drawKey(int key, KDContext* ctx, KDRect rect) const;
  KDColor keyColor(Ion::Keyboard::Key key) const;
  constexpr static int k_margin = 4;
  constexpr static int k_smallSquareSize = 8;
  constexpr static int k_bigSquareSize = 14;
  constexpr static int k_smallRectHeight = 8;
  constexpr static int k_smallRectWidth = 16;
  constexpr static int k_bigRectHeight = 14;
  constexpr static int k_bigRectWidth = 20;
  KeyboardModel m_keyboardModel;
};
}  // namespace HardwareTest

#endif
