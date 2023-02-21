#ifndef SHARED_INFERENCE_H
#define SHARED_INFERENCE_H

#include <apps/i18n.h>
#include <poincare/layout.h>
#include <poincare/preferences.h>

#include "memoized_curve_view_range.h"

// Inference is the common ancestor of Statistic and Distribution

namespace Shared {

struct ParameterRepresentation {
  Poincare::Layout m_symbol;
  I18n::Message m_description;
};

class Inference : public MemoizedCurveViewRange {
 public:
  Inference();
  virtual ~Inference() = default;

  virtual I18n::Message title() const = 0;

  // Input parameters
  virtual int numberOfParameters() = 0;
  virtual double parameterAtIndex(int i) const {
    return const_cast<Inference *>(this)->parametersArray()[i];
  }
  virtual void setParameterAtIndex(double f, int i) {
    parametersArray()[i] = f;
  }
  Poincare::Layout parameterSymbolAtIndex(int i) const {
    return paramRepresentationAtIndex(i).m_symbol;
  }
  I18n::Message parameterDefinitionAtIndex(int i) const {
    return paramRepresentationAtIndex(i).m_description;
  }
  virtual bool authorizedParameterAtIndex(double x, int i) const;

  // Evaluation of distribution
  virtual float evaluateAtAbscissa(float x) const = 0;
  virtual double cumulativeDistributiveFunctionAtAbscissa(double x) const = 0;
  virtual double cumulativeDistributiveInverseForProbability(
      double probability) const = 0;

  constexpr static float k_displayTopMarginRatio = 0.05f;
  constexpr static float k_displayLeftMarginRatio = 0.05f;
  constexpr static float k_displayRightMarginRatio = 0.05f;
  constexpr static float k_displayBottomMarginRatio = 0.2f;

 protected:
  void computeCurveViewRange();
  virtual float computeXMin() const = 0;
  virtual float computeXMax() const = 0;
  virtual float computeYMin() const {
    return -k_displayBottomMarginRatio * computeYMax();
  }
  virtual float computeYMax() const = 0;
  virtual ParameterRepresentation paramRepresentationAtIndex(int i) const = 0;
  virtual double *parametersArray() = 0;

 private:
  static const Poincare::Range1D DefaultYRange() {
    return Poincare::Range1D(-k_displayBottomMarginRatio, 1);
  }
};

}  // namespace Shared

#endif
