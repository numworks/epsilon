#include <emscripten/bind.h>
#include <poincare/numeric/solver.h>
#include <poincare/old/context.h>

#include "typed_expression.h"

using namespace emscripten;

namespace Poincare::JSBridge {

Coordinate2D<double> nextRoot(Solver<double>& solver,
                              const TypedSystemFunction& e) {
  return solver.nextRoot(e.tree()).xy;
}

Coordinate2D<double> nextMinimum(Solver<double>& solver,
                                 const TypedSystemFunction& e) {
  return solver.nextMinimum(e.tree()).xy;
}

Coordinate2D<double> nextMaximum(Solver<double>& solver,
                                 const TypedSystemFunction& e) {
  return solver.nextMaximum(e.tree()).xy;
}

Coordinate2D<double> nextIntersection(Solver<double>& solver,
                                      const TypedSystemFunction& e1,
                                      const TypedSystemFunction& e2) {
  return solver.nextIntersection(e1.tree(), e2.tree()).xy;
}

EMSCRIPTEN_BINDINGS(solver) {
  class_<Coordinate2D<double>>("PCR_Coordinate2D")
      .constructor<double, double>()
      .constructor<>()
      .function("x", &Coordinate2D<double>::x)
      .function("y", &Coordinate2D<double>::y);
  class_<Solver<double>>("PCR_Solver")
      .constructor<double, double, Context*>()
      .function("stretch", &Solver<double>::stretch)
      .function("setSearchStep", &Solver<double>::setSearchStep)
      .function("nextRoot", &nextRoot)
      .function("nextMinimum", &nextMinimum)
      .function("nextMaximum", &nextMaximum)
      .function("nextIntersection", &nextIntersection);
}

}  // namespace Poincare::JSBridge
