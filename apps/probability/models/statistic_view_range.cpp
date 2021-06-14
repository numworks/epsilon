#include "statistic_view_range.h"

#include <ion/display.h>
#include <math.h>

#include "probability/app.h"
#include "probability/gui/statistic_curve_view.h"
#include "probability/helpers.h"

namespace Probability {

float StatisticViewRange::xMin() const {
  return computeXRange().min;
}

float StatisticViewRange::xMax() const {
  return computeXRange().max;
}

float StatisticViewRange::yMin() const {
  return computeYRange().min;
}

float StatisticViewRange::yMax() const {
  return computeYRange().max;
}

StatisticViewRange::Range StatisticViewRange::computeXRange() const {
  // TODO this is called +100 times, memoize ?
  if (App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Tests) {
    float zAlpha = m_statistic->zAlpha();
    float z = m_statistic->testCriticalValue();
    if (m_statistic->hypothesisParams()->op() == HypothesisParams::ComparisonOperator::Different) {
      zAlpha = fabs(zAlpha);
      z = fabs(z);
      Range r = computeTestXRange(z, zAlpha);
      if (m_isLeftRange) {
        // Flip
        r = Range{-r.max, -r.min};
      }
      return r;
    } else {
      return computeTestXRange(z, zAlpha);
    }
  }
  // Confidence interval
  float center = m_statistic->estimate();
  float delta = m_statistic->standardError();
  constexpr static float factor = 4;
  return Range{center - factor * delta, center + factor * delta};
}

StatisticViewRange::Range StatisticViewRange::computeTestXRange(float z, float zAlpha) const {
  // |-----------------|-------------------|-------
  // <-k_marginSide-> min <-k_areaWidth-> max
  float min = fminf(zAlpha, z);
  float max = fmaxf(zAlpha, z);
  float pixelWidth = (max - min) / k_areaWidth;
  return Range{min - k_marginSide * pixelWidth,
               min + (m_statisticCurveView->bounds().width() - k_marginSide) * pixelWidth};
}

StatisticViewRange::Range StatisticViewRange::computeYRange() const {
  if (App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Intervals) {
    return Range{-0.1, 0.5};
  }
  float zAlpha = m_statistic->zAlpha();
  float z = m_statistic->testCriticalValue();
  float min =
      fminf(m_statistic->normedDensityFunction(z), m_statistic->normedDensityFunction(zAlpha));
  float pixelHeight = min / k_areaHeight;
  return Range{-25 * pixelHeight, 100 * pixelHeight};  // TODO access or compute view height
}

}  // namespace Probability
