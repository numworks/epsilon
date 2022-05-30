#ifndef INFERENCE_STATISTIC_STATISTIC_CURVE_VIEW_H
#define INFERENCE_STATISTIC_STATISTIC_CURVE_VIEW_H

#include <apps/shared/curve_view.h>
#include <escher/palette.h>
#include <poincare/coordinate_2D.h>
#include <poincare/layout.h>

#include "inference/models/statistic/statistic.h"

namespace Inference {

class StatisticCurveView : public Shared::CurveView {
public:
  using Shared::CurveView::CurveView;
  void drawRect(KDContext * ctx, KDRect rect) const override;

protected:
  constexpr static KDCoordinate k_axisWidth = 1;
  constexpr static KDColor k_backgroundColor = Escher::Palette::WallScreen;

  char * label(Axis axis, int index) const override;
  KDCoordinate drawGraduationAtPosition(KDContext * ctx, float horizontalPosition, float verticalPosition = 0.0f, KDColor color = KDColorBlack, KDCoordinate halfHeight = k_graduationHalfHeight, KDCoordinate axisThickness = k_axisWidth) const;

private:
  constexpr static KDCoordinate k_graduationHalfHeight = (k_labelGraduationLength - k_axisWidth) / 2;

  char m_labels[k_maxNumberOfXLabels][k_labelBufferMaxSize];
};

}  // namespace Inference

#endif /* INFERENCE_STATISTIC_STATISTIC_CURVE_VIEW_H */
