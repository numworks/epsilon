#ifndef INFERENCE_PROBABILITY_DISTRIBUTION_CURVE_VIEW_H
#define INFERENCE_PROBABILITY_DISTRIBUTION_CURVE_VIEW_H

#include <apps/shared/curve_view.h>
#include <poincare/coordinate_2D.h>

#include "distributions/models/probability/calculation/calculation.h"
#include "distributions/models/probability/distribution/distribution.h"

namespace Inference {

class DistributionCurveView : public Shared::CurveView {
 public:
  DistributionCurveView(Distribution * distribution, Calculation * calculation) : CurveView(distribution, nullptr, nullptr, nullptr),
      m_labels{},
      m_distribution(distribution),
      m_calculation(calculation) {
        assert(distribution != nullptr);
      }

  void reload(bool resetInterrupted = false, bool force = false) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;

 protected:
  char * label(Axis axis, int index) const override;

 private:
  static float EvaluateAtAbscissa(float abscissa, void * model, void * context);
  static Poincare::Coordinate2D<float> EvaluateXYAtAbscissa(float abscissa, void * model, void * context);
  constexpr static KDColor k_backgroundColor = Escher::Palette::WallScreen;
  void drawStandardNormal(KDContext * ctx, KDRect rect, float colorLowerBound, float colorUpperBound) const;
  char m_labels[k_maxNumberOfXLabels][k_labelBufferMaxSize];
  Distribution * m_distribution;
  Calculation * m_calculation;
};

}  // namespace Inference

#endif /* INFERENCE_PROBABILITY_DISTRIBUTION_CURVE_VIEW_H */
