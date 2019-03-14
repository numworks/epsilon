#ifndef PROBABILITY_LAW_CURVE_VIEW_H
#define PROBABILITY_LAW_CURVE_VIEW_H

#include "../shared/curve_view.h"
#include "../constant.h"
#include "law/law.h"
#include "calculation/calculation.h"
#include <escher.h>
#include <poincare/print_float.h>

namespace Probability {

class LawCurveView : public Shared::CurveView {
public:
  LawCurveView(Law * law, Calculation * calculation) :
    CurveView(law, nullptr, nullptr, nullptr),
    m_labels{},
    m_law(law),
    m_calculation(calculation)
  {
    assert(law != nullptr);
    assert(calculation != nullptr);
  }

  void reload() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  char * label(Axis axis, int index) const override;
private:
  static float EvaluateAtAbscissa(float abscissa, void * model, void * context);
  static constexpr KDColor k_backgroundColor = Palette::WallScreen;
  void drawStandardNormal(KDContext * ctx, KDRect rect, float colorLowerBound, float colorUpperBound) const;
  char m_labels[k_maxNumberOfXLabels][k_labelBufferMaxSize];
  Law * m_law;
  Calculation * m_calculation;
};

}

#endif
