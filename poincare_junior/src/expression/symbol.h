#ifndef POINCARE_EXPRESSION_SYMBOL_H
#define POINCARE_EXPRESSION_SYMBOL_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Symbol final {
 public:
  static uint8_t Length(const Tree* node) { return node->nodeValue(0); }
  static void GetName(const Tree* node, char* buffer, size_t bufferSize);
  static const char* NonNullTerminatedName(const Tree* node);
};

}  // namespace PoincareJ

#endif
