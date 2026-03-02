#pragma once

namespace Poincare {

struct ApproximationPolicy {
  /* Only numerical values are approximated, the rest is preserved (including
   * randoms, variables, units, function symbols, depLists). For example "pi*x +
   * 1/3" will be transformed into "3.14*x + 0.33". This is useful to optimize
   * an expression for successive approximations (for example in function graph
   * or solver). This parameter is only relevant when the return type is an
   * expression (i.e. a Tree*). */
  bool onlyReplaceNumerical = false;
  /* Optimize out some validation steps (for now only the expression dimension
   * check is optimized out) for a shorter computing time. These checks are
   * asserted instead. Particularly relevant when the approximation is called
   * multiple times (e.g. Solver or Grapher) */
  bool optimize = false;
};

}  // namespace Poincare
