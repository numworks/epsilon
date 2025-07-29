#ifndef POINCARE_HELPERS_PARSER_H
#define POINCARE_HELPERS_PARSER_H

namespace Poincare::ParserHelper {

struct ParsingParameters {
  /* If true:
   *  f(x) is understood as a function and not f*(x).
   *  abc is understood as abc and not a*b*c.
   * This sets back to false when an '=' symbol is reached */
  bool isAssignment = false;
  // If true, "g" will never be parsed as a unit (for solver)
  bool forceUnitUnderscore = false;
  /* If true, the layout w_n = [] is allowed. This is useful for turning
   * 2D sequences into 1D sequences when inserting from toolbox.
   * TODO: It's very niche. Should probably be refactored at the same time as
   * nullptr parsing. */
  bool allowEmptySequence = false;
};

}  // namespace Poincare::ParserHelper
#endif
