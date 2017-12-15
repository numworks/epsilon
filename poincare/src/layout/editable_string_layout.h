#ifndef POINCARE_EDITABLE_STRING_LAYOUT_H
#define POINCARE_EDITABLE_STRING_LAYOUT_H

#include "string_layout.h"
#include <string.h>

namespace Poincare {

class EditableStringLayout : public StringLayout {
public:
  using StringLayout::StringLayout;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
};

}

#endif
