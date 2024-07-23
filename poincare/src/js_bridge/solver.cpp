#include <emscripten/bind.h>
#include <poincare/numeric/solver.h>
#include <poincare/old/context.h>

using namespace emscripten;

namespace Poincare::JSBridge {

// For default parameter
Coordinate2D<double> nextIntersection(Internal::Solver<double>* solver,
                                      const Internal::Tree* e1,
                                      const Internal::Tree* e2) {
  return solver->nextIntersection(e1, e2);
}

EMSCRIPTEN_BINDINGS(solver) {
  class_<Coordinate2D<double>>("PCR_Coordinate2D")
      .function("x", &Coordinate2D<double>::x)
      .function("y", &Coordinate2D<double>::y);
  class_<Internal::Solver<double>>("PCR_Solver")
      .constructor<double, double, Context*>()
      .function("stretch", &Internal::Solver<double>::stretch)
      .function("setSearchStep", &Internal::Solver<double>::setSearchStep)
      .function("nextRoot",
                select_overload<Coordinate2D<double>(const Internal::Tree*)>(
                    &Internal::Solver<double>::nextRoot),
                allow_raw_pointers())
      .function("nextMinimum",
                select_overload<Coordinate2D<double>(const Internal::Tree*)>(
                    &Internal::Solver<double>::nextMinimum),
                allow_raw_pointers())
      .function("nextMaximum",
                select_overload<Coordinate2D<double>(const Internal::Tree*)>(
                    &Internal::Solver<double>::nextMaximum),
                allow_raw_pointers())
      .function("nextIntersection", &nextIntersection, allow_raw_pointers());
}

}  // namespace Poincare::JSBridge
