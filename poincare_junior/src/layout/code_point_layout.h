#ifndef POINCARE_JUNIOR_LAYOUT_CODE_POINT_LAYOUT_H
#define POINCARE_JUNIOR_LAYOUT_CODE_POINT_LAYOUT_H

#include <ion/unicode/code_point.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class CodePointLayout {
 public:
  static CodePoint GetCodePoint(const Tree* node);
  static CodePoint GetCombinedCodePoint(const Tree* node);
  static Tree* Push(CodePoint cp);

  // Print name in buffer and return end
  static char* CopyName(const Tree* node, char* buffer, size_t bufferSize);

  static bool IsCodePoint(const Tree* node, CodePoint cp);
};

}  // namespace PoincareJ

#endif
