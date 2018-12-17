#ifndef APPS_CONSTANT_H
#define APPS_CONSTANT_H

#include <escher/text_field.h>

class Constant {
public:
  constexpr static int LargeNumberOfSignificantDigits = 7;
  constexpr static int MediumNumberOfSignificantDigits = 5;
  constexpr static int ShortNumberOfSignificantDigits = 4;
  constexpr static int MaxSerializedExpressionSize = 2*::TextField::maxBufferSize();
};

#endif
