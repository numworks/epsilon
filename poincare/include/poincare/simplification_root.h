#ifndef POINCARE_SIMPLIFICATION_ROOT_H
#define POINCARE_SIMPLIFICATION_ROOT_H

#include <poincare/static_hierarchy.h>

namespace Poincare {

class SimplificationRoot : public StaticHierarchy<1> {
public:
  SimplificationRoot(Expression * e) : StaticHierarchy<1>(&e, false) {
    e->setParent(this);
  }
  ~SimplificationRoot() {
    detachOperand(operand(0));
    /* We don't want to clone the expression provided at construction.
     * So we don't want it to be deleted when we're destroyed (parent destructor). */
  }
  Expression * clone() const override { return nullptr; }
  Type type() const override { return Expression::Type::Undefined; }
  Expression * immediateSimplify() override { return this; }
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override {
    return nullptr;
  }
  int writeTextInBuffer(char * buffer, int bufferSize) const override { return 0; }
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return nullptr;
  }
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return nullptr;
  }
};

}

#endif
