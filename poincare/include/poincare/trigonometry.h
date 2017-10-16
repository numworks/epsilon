#ifndef POINCARE_TRIGONOMETRY_H
#define POINCARE_TRIGONOMETRY_H

#include <poincare/expression.h>
#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/evaluation_engine.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class Trigonometry : public StaticHierarchy<1>  {
  using StaticHierarchy<1>::StaticHierarchy;
public:
  Expression * immediateSimplify() override;
  constexpr static int k_numberOfEntries = 24;
protected:
  enum class Function {
    Cosine = 0,
    Sine = 1,
  };
  virtual Function trigonometricFunctionType() const = 0;
  static Expression * table(const Expression * e, Function f, bool inverse); // , Function f, bool inverse
};

}

#endif
