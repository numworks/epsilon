#ifndef APPS_CONSTANT_H
#define APPS_CONSTANT_H

#include <escher/text_field.h>

class Constant {
public:
  constexpr static int MaxSerializedExpressionSize = 2*Escher::TextField::MaxBufferSize();
};

#endif
