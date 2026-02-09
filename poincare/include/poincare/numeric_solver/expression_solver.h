#pragma once

#include <poincare/symbol_context.h>
#include <poincare/tree.h>

#include "solver.h"

/* TODO_PCJ: Change signatures to preparedFunctions instead of Trees. */
namespace Poincare::NumericSolver {

class ExpressionSolver : public Solver<double> {
 public:
  ExpressionSolver(double xStart, double xEnd, const Internal::Tree* e,
                   const SymbolContext& context,
                   Solver<double>::BracketTest test,
                   Solver<double>::HoneResult hone);

 protected:
  const Internal::Tree* getTree() const {
    return reinterpret_cast<const Internal::Tree*>(m_auxiliary);
  }

  const SymbolContext& m_context;
};

// Specific implementations for each interest

// Search for Minimums and Maximums
class ExtremumSolver : public ExpressionSolver {
 public:
  ExtremumSolver(double xStart, double xEnd, const Internal::Tree* e,
                 const SymbolContext& context);
};

class RootSolver : public ExpressionSolver {
 public:
  RootSolver(double xStart, double xEnd, const Internal::Tree* e,
             const SymbolContext& context);

  Solution<double> next() override;

 private:
  /* Stores the root and a pointer to the minimal sub-expression
   * that evaluates to zero (mainly used for dependencies handling) */
  struct DetailedRoot {
    double x;
    const Internal::Tree* nullDescendant = nullptr;
  };
  constexpr static DetailedRoot k_nanRoot = {.x = k_NAN,
                                             .nullDescendant = nullptr};

  DetailedRoot nextDetailedRoot();
  // Find the next root of the child which is also a root of the parent.
  DetailedRoot nextRootSearchingInChild(int childIndex) const;
  typedef bool (*ExpressionTestAuxiliary)(const Internal::Tree*,
                                          const SymbolContext& symbolContext);
  /* Find the next root among the children satisfying the test, which is also a
   * root of the parent. */
  DetailedRoot nextRootSearchingInChildren(ExpressionTestAuxiliary test) const;
  DetailedRoot nextRootInMultiplication() const;
  DetailedRoot nextRootInAddition() const;
  DetailedRoot nextRootInDependency() const;

  DetailedRoot registerDetailedRoot(DetailedRoot root) {
    registerSolution(SolutionFromDetailedRoot(root));
    return root;
  }

  static Solution<double> SolutionFromDetailedRoot(DetailedRoot root) {
    return std::isfinite(root.x) ? Solution<double>{root.x, 0., Interest::Root}
                                 : Solution<double>{};
  }
};

class IntersectionSolver : public RootSolver {
 public:
  IntersectionSolver(double xStart, double xEnd, const Internal::Tree* f,
                     const Internal::Tree* g, bool areAlongSameAxis,
                     const SymbolContext& context);
  IntersectionSolver(double xStart, double xEnd, const Internal::Tree* f,
                     const Internal::Tree* g, const SymbolContext& context)
      : IntersectionSolver(xStart, xEnd, f, g, true, context) {}

  ~IntersectionSolver();  // Remove memoized diff
  IntersectionSolver(const IntersectionSolver&);
  IntersectionSolver& operator=(const IntersectionSolver&);

  Solution<double> next() override;

 private:
  IntersectionSolver(double xStart, double xEnd, const Internal::Tree* f,
                     const Internal::Tree* g, Internal::Tree* diff,
                     bool areAlongSameAxis, const SymbolContext& context);

  const Internal::Tree* m_f;
  const Internal::Tree* m_g;
  bool m_alongSameAxis;
};

class DiscontinuitySolver : public ExpressionSolver {
 public:
  DiscontinuitySolver(double xStart, double xEnd, const Internal::Tree* e,
                      const SymbolContext& context);

 private:
  void honeAndRoundSolution(Coordinate2D<double> start,
                            Coordinate2D<double> end,
                            Interest interest) override;
};

}  // namespace Poincare::NumericSolver
