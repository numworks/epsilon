#ifndef POINCARE_TRIGONOMETRIC_FUNCTION_H
#define POINCARE_TRIGONOMETRIC_FUNCTION_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/evaluation_engine.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class TrigonometricFunction : public StaticHierarchy<1>  {
  using StaticHierarchy<1>::StaticHierarchy;
public:
  Expression * immediateSimplify() override;
private:
  virtual Trigonometry::Function trigonometricFunctionType() const = 0;
};

}

#endif
