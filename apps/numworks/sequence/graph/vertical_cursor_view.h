#ifndef SEQUENCE_VERTICAL_CURSOR_VIEW_H
#define SEQUENCE_VERTICAL_CURSOR_VIEW_H

#include <escher.h>

namespace Sequence {

class VerticalCursorView : public View {
public:
  using View::View;
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}

#endif
