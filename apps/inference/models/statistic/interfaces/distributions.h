#ifndef INFERENCE_MODELS_STATISTIC_INTERFACES_DISTRIBUTIONS_H
#define INFERENCE_MODELS_STATISTIC_INTERFACES_DISTRIBUTIONS_H

#include <poincare/layout.h>
#include <poincare/statistics/distribution.h>

namespace Inference {

class Distribution {
 public:
  virtual const char* criticalValueSymbol() const = 0;
  virtual Poincare::Layout criticalValueSymbolLayout() const = 0;
  float canonicalDensityFunction(float x, double degreesOfFreedom) const;
  double cumulativeNormalizedDistributionFunction(
      double x, double degreesOfFreedom) const;
  double cumulativeNormalizedInverseDistributionFunction(
      double proba, double degreesOfFreedom) const;

  virtual float yMax(double degreesOfFreedom) const = 0;

 protected:
  virtual Poincare::Distribution::Type distributionType() const = 0;
  const Poincare::Distribution* distribution() const {
    return Poincare::Distribution::Get(distributionType());
  }
  virtual const double* constParametersArray(
      double* degreesOfFreedom) const = 0;
};

class DistributionT : public Distribution {
 public:
  const char* criticalValueSymbol() const override { return "t"; }
  Poincare::Layout criticalValueSymbolLayout() const override {
    return Poincare::Layout::String("t", -1);
  }

  float yMax(double degreesOfFreedom) const override;

 protected:
  Poincare::Distribution::Type distributionType() const override {
    return Poincare::Distribution::Type::Student;
  }

  const double* constParametersArray(double* degreesOfFreedom) const override {
    assert(*degreesOfFreedom > 0);
    return degreesOfFreedom;
  }
};

class DistributionZ : public Distribution {
 public:
  const char* criticalValueSymbol() const override { return "z"; }
  Poincare::Layout criticalValueSymbolLayout() const override {
    return Poincare::Layout::String("z", -1);
  }

  float yMax(double degreesOfFreedom) const override;

 protected:
  Poincare::Distribution::Type distributionType() const override {
    return Poincare::Distribution::Type::Normal;
  }
  constexpr static double k_params[] = {0, 1};
  const double* constParametersArray(double* degreesOfFreedom) const override {
    return k_params;
  }
};

class DistributionChi2 : public Distribution {
 public:
  const char* criticalValueSymbol() const override { return "χ2"; }
  Poincare::Layout criticalValueSymbolLayout() const override;

  float xMin(double degreesOfFreedom) const;
  float xMax(double degreesOfFreedom) const;
  float yMax(double degreesOfFreedom) const override;

 protected:
  Poincare::Distribution::Type distributionType() const override {
    return Poincare::Distribution::Type::ChiSquared;
  }

  const double* constParametersArray(double* degreesOfFreedom) const override {
    assert(*degreesOfFreedom > 0);
    return degreesOfFreedom;
  }
};

}  // namespace Inference

#endif
