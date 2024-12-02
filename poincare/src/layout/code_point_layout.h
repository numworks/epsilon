#ifndef POINCARE_LAYOUT_CODE_POINT_LAYOUT_H
#define POINCARE_LAYOUT_CODE_POINT_LAYOUT_H

#include <omg/code_point.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class CodePointLayout {
 public:
  static CodePoint GetCodePoint(const Tree* l);
  static CodePoint GetCombinedCodePoint(const Tree* l);
  static Tree* Push(CodePoint codePoint);
  static Tree* PushCombined(CodePoint codePoint, CodePoint combinedCodePoint);

  // Print name in buffer and return end
  static char* CopyName(const Tree* l, char* buffer, size_t bufferSize);

  static bool IsCodePoint(const Tree* l, CodePoint codePoint);
  static bool IsCombinedCodePoint(const Tree* l, CodePoint codePoint,
                                  CodePoint combinedCodePoint);
};

}  // namespace Poincare::Internal

#endif
