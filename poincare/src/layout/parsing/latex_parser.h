#ifndef POINCARE_LAYOUT_LATEX_PARSER_LATEX_TO_LAYOUT_H
#define POINCARE_LAYOUT_LATEX_PARSER_LATEX_TO_LAYOUT_H

#include <poincare/src/layout/rack.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

namespace LatexParser {

Tree* LatexToLayout(const char* latexString);
char* LayoutToLatex(const Rack* rack, char* buffer, char* end,
                    bool withThousandsSeparators = false);

}  // namespace LatexParser

}  // namespace Poincare::Internal

#endif
