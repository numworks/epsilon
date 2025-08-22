#pragma once

#include <escher/text_field.h>

class Constant {
 public:
  constexpr static int MaxSerializedExpressionSize =
      2 * Escher::TextField::MaxBufferSize();
};
