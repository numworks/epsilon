#ifndef POINCARE_LOGARITHM_H
#define POINCARE_LOGARITHM_H

#include <poincare/layout_engine.h>
#include <poincare/bounded_static_hierarchy.h>
#include <poincare/integer.h>

namespace Poincare {

class Logarithm : public BoundedStaticHierarchy<2>  {
  using BoundedStaticHierarchy<2>::BoundedStaticHierarchy;
  friend class NaperianLogarithm;
public:
  Type type() const override;
  Expression * clone() const override;
private:
  /* Layout */
  ExpressionLayout * createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "log");
  }
  /* Simplification */
  Expression * shallowReduce(Context & context, AngleUnit angleUnit) override;
  Expression * simpleShallowReduce(Context & context, AngleUnit angleUnit);
  Expression * shallowBeautify(Context & context, AngleUnit angleUnit) override;
  bool parentIsAPowerOfSameBase() const;
  Expression * splitInteger(Integer i, bool isDenominator, Context & context, AngleUnit angleUnit);
  /* Evaluation */
  template<typename T> static std::complex<T> computeOnComplex(const std::complex<T> c, AngleUnit angleUnit);
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> * templatedApproximate(Context& context, AngleUnit angleUnit) const;
};

}

#endif
