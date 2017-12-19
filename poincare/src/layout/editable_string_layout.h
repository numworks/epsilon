#ifndef POINCARE_EDITABLE_STRING_LAYOUT_H
#define POINCARE_EDITABLE_STRING_LAYOUT_H

#include "string_layout.h"
#include <string.h>

namespace Poincare {

class EditableStringLayout : public StringLayout {
public:
  using StringLayout::StringLayout;
  ExpressionLayout * clone() const override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
private:
  void moveCursorInsideAtDirection (
    VerticalDirection direction,
    ExpressionLayoutCursor * cursor,
    ExpressionLayout ** childResult,
    void * resultPosition,
    int * resultPositionInside,
    int * resultScore) override;
};

}

#endif
