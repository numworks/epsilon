#ifndef INFERENCE_MODELS_STATISTIC_INTERFACES_DISTRIBUTIONS_H
#define INFERENCE_MODELS_STATISTIC_INTERFACES_DISTRIBUTIONS_H

#include <poincare/layout_helper.h>

namespace Inference {

class Distribution {
 public:
  virtual const char* criticalValueSymbol() const = 0;
  virtual Poincare::Layout criticalValueSymbolLayout() const = 0;
  virtual float canonicalDensityFunction(float x,
                                         double degreesOfFreedom) const = 0;
  virtual double cumulativeNormalizedDistributionFunction(
      double x, double degreesOfFreedom) const = 0;
  virtual double cumulativeNormalizedInverseDistributionFunction(
      double proba, double degreesOfFreedom) const = 0;

  virtual float yMax(double degreesOfFreedom) const = 0;
};

class DistributionT : public Distribution {
 public:
  const char* criticalValueSymbol() const override { return "t"; }
  Poincare::Layout criticalValueSymbolLayout() const override {
    return Poincare::Layout::String("t", -1);
  }
  float canonicalDensityFunction(float x,
                                 double degreesOfFreedom) const override;
  double cumulativeNormalizedDistributionFunction(
      double x, double degreesOfFreedom) const override;
  double cumulativeNormalizedInverseDistributionFunction(
      double proba, double degreesOfFreedom) const override;

  float yMax(double degreesOfFreedom) const override;
};

class DistributionZ : public Distribution {
 public:
  const char* criticalValueSymbol() const override { return "z"; }
  Poincare::Layout criticalValueSymbolLayout() const override {
    return Poincare::Layout::String("z", -1);
  }
  float canonicalDensityFunction(float x,
                                 double degreesOfFreedom) const override;
  double cumulativeNormalizedDistributionFunction(
      double x, double degreesOfFreedom) const override;
  double cumulativeNormalizedInverseDistributionFunction(
      double proba, double degreesOfFreedom) const override;

  float yMax(double degreesOfFreedom) const override;
};

class DistributionChi2 : public Distribution {
 public:
  const char* criticalValueSymbol() const override { return "χ2"; }
  Poincare::Layout criticalValueSymbolLayout() const override;
  float canonicalDensityFunction(float x,
                                 double degreesOfFreedom) const override;
  double cumulativeNormalizedDistributionFunction(
      double x, double degreesOfFreedom) const override;
  double cumulativeNormalizedInverseDistributionFunction(
      double proba, double degreesOfFreedom) const override;

  float xMin(double degreesOfFreedom) const;
  float xMax(double degreesOfFreedom) const;
  float yMax(double degreesOfFreedom) const override;
};

}  // namespace Inference

#endif
