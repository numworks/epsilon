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
    m_xMax = xMin + powf(10.0f, floorf(log10f(fabsf(xMin)))-1.0f);
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
}

void MemoizedCurveViewRange::setXMax(float xMax) {
  if (isnan(xMax)) {
    return;
  }
  m_xMax = xMax;
  if (m_xMin >= m_xMax) {
    m_xMin = xMax - powf(10.0f, floorf(log10f(fabsf(xMax)))-1.0f);
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
}

void MemoizedCurveViewRange::setYMin(float yMin) {
  if (isnan(yMin)) {
    return;
  }
  m_yMin = yMin;
  if (m_yMin >= m_yMax) {
    m_yMax = yMin + powf(10.0f, floorf(log10f(fabsf(yMin)))-1.0f);
  }
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void MemoizedCurveViewRange::setYMax(float yMax) {
  if (isnan(yMax)) {
    return;
  }
  m_yMax = yMax;
  if (m_yMin >= m_yMax) {
    m_yMin = yMax - + powf(10.0f, floorf(log10f(fabsf(yMax)))-1.0f);

  }
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

}
