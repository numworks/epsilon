#ifndef ESCHER_TEXT_CURSOR_VIEW_H
#define ESCHER_TEXT_CURSOR_VIEW_H

#include <escher/view.h>

class TextCursorView : public View {
public:
  using View::View;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
};

#endif

