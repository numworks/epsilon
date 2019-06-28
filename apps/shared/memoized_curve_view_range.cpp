#include "memoized_curve_view_range.h"
#include <cmath>
#include <assert.h>
#include <ion.h>

namespace Shared {

MemoizedCurveViewRange::MemoizedCurveViewRange() :
  m_xMin(-10.0f),
  m_xMax(10.0f),
  m_yMin(-10.0f),
  m_yMax(10.0f),
  m_xGridUnit(2.0f),
  m_yGridUnit(2.0f)
{
}

void MemoizedCurveViewRange::setXMin(float xMin) {
  if (std::isnan(xMin)) {
    return;
  }
  m_xMin = xMin;
  if (m_xMin >= m_xMax) {
    m_xMax = xMin + defaultRangeLengthFor(xMin);
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMax - m_xMin);
}

void MemoizedCurveViewRange::setXMax(float xMax) {
  if (std::isnan(xMax)) {
    return;
  }
  m_xMax = xMax;
  if (m_xMin >= m_xMax) {
    m_xMin = xMax - defaultRangeLengthFor(xMax);
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMax - m_xMin);
}

void MemoizedCurveViewRange::setYMin(float yMin) {
  if (std::isnan(yMin)) {
    return;
  }
  m_yMin = yMin;
  if (m_yMin >= m_yMax) {
    m_yMax = yMin + defaultRangeLengthFor(yMin);
  }
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMax - m_yMin);
}

void MemoizedCurveViewRange::setYMax(float yMax) {
  if (std::isnan(yMax)) {
    return;
  }
  m_yMax = yMax;
  if (m_yMin >= m_yMax) {
    m_yMin = yMax - defaultRangeLengthFor(yMax);
  }
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMax - m_yMin);
}

float MemoizedCurveViewRange::defaultRangeLengthFor(float position) const {
  return std::pow(10.0f, std::floor(std::log10(std::fabs(position)))-1.0f);
}

}
