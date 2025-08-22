#pragma once

#include <poincare/src/layout/rack.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class LatexParser {
 public:
  static Tree* LatexToLayout(const char* latexString);
  static char* LayoutToLatex(const Rack* rack, char* buffer, char* end,
                             bool withThousandsSeparators = false);

 private:
  static char* LayoutToLatexWithExceptions(const Rack* rack, char* buffer,
                                           const char* end,
                                           bool withThousandsSeparators);
};

}  // namespace Poincare::Internal
