#ifndef POINCARE_LAYOUT_CODE_POINT_LAYOUT_H
#define POINCARE_LAYOUT_CODE_POINT_LAYOUT_H

#include <ion/unicode/code_point.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class CodePointLayout {
 public:
  static CodePoint GetCodePoint(const Tree* node);
  static CodePoint GetCombinedCodePoint(const Tree* node);
  static Tree* Push(CodePoint cp);

  // Print name in buffer and return end
  static char* CopyName(const Tree* node, char* buffer, size_t bufferSize);

  static bool IsCodePoint(const Tree* node, CodePoint cp);
};

}  // namespace Poincare::Internal

#endif
