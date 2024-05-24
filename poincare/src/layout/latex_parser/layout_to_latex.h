#ifndef POINCARE_LAYOUT_LATEX_PARSER_LAYOUT_TO_LATEX_H
#define POINCARE_LAYOUT_LATEX_PARSER_LAYOUT_TO_LATEX_H

#include <poincare/src/layout/rack.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

namespace LatexParser {

class LayoutToLatex {
 public:
  static char* Parse(const Rack* rack, char* buffer, char* end);

 private:
  // static void ParseOnRackUntilIdentifier(Rack* parent, const char** start,
  //                                       const char* endIdentifier);
  // static Tree* NextToken(const char** start);
};

}  // namespace LatexParser

}  // namespace Poincare::Internal

#endif
