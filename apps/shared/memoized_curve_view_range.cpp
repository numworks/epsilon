#include "memoized_curve_view_range.h"
#include <math.h>
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

float MemoizedCurveViewRange::xMin() {
  return m_xMin;
}

float MemoizedCurveViewRange::xMax() {
  return m_xMax;
}

float MemoizedCurveViewRange::yMin() {
  return m_yMin;
}

float MemoizedCurveViewRange::yMax() {
  return m_yMax;
}

float MemoizedCurveViewRange::xGridUnit() {
  return m_xGridUnit;
}

float MemoizedCurveViewRange::yGridUnit() {
  return m_yGridUnit;
}

void MemoizedCurveViewRange::setXMin(float xMin) {
  if (isnan(xMin)) {
    return;
  }
  m_xMin = xMin;
  if (m_xMin >= m_xMax) {
    m_xMax = xMin + pow(10.0f, floor(log10(fabs(xMin)))-1.0f);
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
}

void MemoizedCurveViewRange::setXMax(float xMax) {
  if (isnan(xMax)) {
    return;
  }
  m_xMax = xMax;
  if (m_xMin >= m_xMax) {
    m_xMin = xMax - pow(10.0f, floor(log10(fabs(xMax)))-1.0f);
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
}

void MemoizedCurveViewRange::setYMin(float yMin) {
  if (isnan(yMin)) {
    return;
  }
  m_yMin = yMin;
  if (m_yMin >= m_yMax) {
    m_yMax = yMin + pow(10.0f, floor(log10(fabs(yMin)))-1.0f);
  }
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void MemoizedCurveViewRange::setYMax(float yMax) {
  if (isnan(yMax)) {
    return;
  }
  m_yMax = yMax;
  if (m_yMin >= m_yMax) {
    m_yMin = yMax - + pow(10.0f, floor(log10(fabs(yMax)))-1.0f);

  }
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

}
