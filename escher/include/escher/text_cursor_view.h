#ifndef ESCHER_TEXT_CURSOR_VIEW_H
#define ESCHER_TEXT_CURSOR_VIEW_H

#include <escher/view.h>

namespace Escher {

class TextCursorView : public View {
public:
  constexpr static KDCoordinate k_width = 1;

  TextCursorView() : m_visible(true) {}
  ~TextCursorView();

  // View
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  KDRect frame() const { return m_frame; }
  void switchVisible();

private:
  bool m_visible;
};

}
#endif
