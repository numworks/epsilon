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
  Expression * clone() const override { return nullptr; }
  int polynomialDegree(char symbolName) const override { return -1; }
  Type type() const override { return Expression::Type::SimplificationRoot; }
  ExpressionLayout * createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    return nullptr;
  }
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override { return 0; }
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    assert(false);
    return nullptr;
  }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    assert(false);
    return nullptr;
  }
private:
  Expression * shallowReduce(Context & context, AngleUnit angleUnit) override { return this; }
};

}

#endif
