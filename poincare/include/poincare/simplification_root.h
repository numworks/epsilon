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
    if (m_operands[0] != nullptr) {
      detachOperand(operand(0));
    }
    /* We don't want to clone the expression provided at construction.
     * So we don't want it to be deleted when we're destroyed (parent destructor). */
  }
  int polynomialDegree(char symbolName) const override { return -1; }
  Type type() const override { return Expression::Type::SimplificationRoot; }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return nullptr;
  }
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { return 0; }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    assert(false);
    return nullptr;
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    assert(false);
    return nullptr;
  }
private:
  ExpressionReference shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override { return this; }
};

}

#endif
