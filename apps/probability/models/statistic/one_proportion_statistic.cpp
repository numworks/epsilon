#include "one_proportion_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

using namespace Poincare;

namespace Probability {

OneProportionStatistic::OneProportionStatistic() {
  m_params[ParamsOrder::X] = 20;
  m_params[ParamsOrder::N] = 50;
  m_hypothesisParams.setFirstParam(0.5);
}

bool OneProportionStatistic::isValidParamAtIndex(int i, float p) {
  switch (i) {
    case ParamsOrder::X:
      return p >= 0;
    case ParamsOrder::N:
      return p > 0;
  }
  return ZStatistic::isValidParamAtIndex(i, p);
}

void OneProportionStatistic::computeTest() {
  float p0 = m_hypothesisParams.firstParam();
  float prop = _pEstimate(x(), n());

  m_zAlpha = absIfNeeded(_zAlpha(m_threshold));
  m_z = _z(p0, prop, n());
  char fakeOp = static_cast<char>(m_hypothesisParams.op());
  m_pValue = _pVal(m_z, fakeOp);
}

void OneProportionStatistic::computeInterval() {
  m_pEstimate = _pEstimate(x(), n());
  m_zCritical = _zCritical(m_threshold);
  m_SE = _SE(m_pEstimate, n());
  m_ME = _ME(m_zCritical, m_SE);
}

ParameterRepr OneProportionStatistic::paramReprAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X: {
      Layout x = CodePointLayout::Builder('x');
      return ParameterRepr{x, I18n::Message::NumberOfSuccesses};
    }
    case ParamsOrder::N: {
      Layout n = CodePointLayout::Builder('n');
      return ParameterRepr{n, I18n::Message::SampleSize};
    }
  }
  assert(false);
  return ParameterRepr{};
}

float OneProportionStatistic::_pEstimate(float x, float n) {
  return x / n;
}

float OneProportionStatistic::_z(float p0, float p, int n) {
  return absIfNeeded((p - p0) / sqrt(p0 * (1 - p0) / n));
}

float OneProportionStatistic::_SE(float pEstimate, int n) {
  return sqrt(pEstimate * (1 - pEstimate) / n);
}

}  // namespace Probability
