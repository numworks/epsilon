#ifndef POINCARE_EXPRESSION_SYMBOL_H
#define POINCARE_EXPRESSION_SYMBOL_H

#include <poincare_junior/src/memory/node.h>

namespace Poincare {

class Symbol final {
public:
  static uint8_t Length(const Node node) { return static_cast<uint8_t>(*node.block()->next()); }
  static void GetName(const Node node, char * buffer, size_t bufferSize);
  static const char * NonNullTerminatedName(const Node node);
};

}

#endif
