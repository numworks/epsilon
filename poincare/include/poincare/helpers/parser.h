#pragma once

namespace Poincare::ParserHelper {

struct ParsingParameters {
  /* If true, the parser will understand any identifier as a user symbol:
   * - If the identifier is followed by a parenthesis, it is understood as:
   *    - a sequence if the identifier is u, v or w (e.g u(n) is u_n, not u*(n))
   *    - a function for other identifiers (e.g. f(x) is not f*(x))
   * - If the identifier is not followed by a parenthesis, it is understood as
   * a CustomIdentifier (e.g. abc is not a*b*c).
   *
   * This is only applied until an '=' symbol is reached, at which point it's
   * assumed that we're entering the assignment declaration.
   *
   * This rule is also automatically applied by the parser right of an arrow
   * (e.g. "x -> f(x)")
   */
  bool isAssignment = false;

  /* This tells the parser that the layout is intentionally structured as-is and
   * should be interpreted as minimally as possible.
   * If preserveInput is true:
   * - Sequences can have an empty definition (e.g. w_n = [])
   *   (useful for turning 2D sequences into 1D sequences when inserting from
   *    toolbox).
   * - abc is understood as abc and not a*b*c (like in isAssignment).
   * - If f is not defined in the context, f(x) is understood as a function
   *   (this rule is slightly different from isAssignment. In the preserveInput
   *    case, if "f" is already defined with another symbol type like variable,
   *    sequence or list, it won't be understood as a function).
   * - g is always understood as a variable and not a unit (even if the context
   *   allows omitting unit underscore).
   * - 1 2/3 is always understood as a mixed fraction and not 1*2/3
   *
   * isAssignment can still be used (in order to undersetand "f(x)=" as a
   * function assignment even if "f" is already defined as a variable).
   */
  bool preserveInput = false;
};

}  // namespace Poincare::ParserHelper
