#ifndef POINCARE_HELPERS_PARSER_H
#define POINCARE_HELPERS_PARSER_H

namespace Poincare::ParserHelper {

struct ParsingParameters {
  /* If true:
   *  f(x) is understood as a function and not f*(x).
   *  abc is understood as abc and not a*b*c.
   * This is only applied until a '=' symbol is reached */
  bool isAssignment = false;
  // If true, "g" will never be parsed as a unit (for solver)
  bool forceUnitUnderscore = false;

  /* This tells the parser that the layout is intentionally structured as-is and
   * should be interpreted as minimally as possible.
   * If preserveInput is true:
   * - Sequences can have an empty definition (e.g. w_n = [])
   *   (useful for turning 2D sequences into 1D sequences when inserting from
   *    toolbox).
   * - f(x) is understood as a function and not f*(x)
   * - abc is understood as abc and not a*b*c.
   *   (the two previous points mean that isAssignment is included in
   *    preserveInput)
   * - g is always understood as a variable and not a unit.
   *   (this means that forceUnitUnderscore is included in preserveInput)
   * - 1 2/3 is always understood as a mixed fraction and not 1*2/3
   */
  bool preserveInput = false;
};

}  // namespace Poincare::ParserHelper
#endif
