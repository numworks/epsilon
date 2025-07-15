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
  // TODO: Add comment explaining what this does. Not clear for me
  bool forceParseSequence = false;
};

}  // namespace Poincare::ParserHelper
#endif
