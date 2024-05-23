#ifndef POINCARE_LAYOUT_LATEX_PARSER_LATEX_TO_LAYOUT_H
#define POINCARE_LAYOUT_LATEX_PARSER_LATEX_TO_LAYOUT_H

#include <poincare/src/layout/rack.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/src/memory/type_block.h>

#include <array>

namespace Poincare::Internal {

namespace LatexParser {

class LatexToLayout {
 public:
  static Tree* Parse(const char* latexString);

 private:
  static void ParseOnRackUntilIdentifier(Rack* parent, const char** start,
                                         const char* endIdentifier);
  static Tree* NextToken(const char** start);
};

}  // namespace LatexParser

}  // namespace Poincare::Internal

#endif
