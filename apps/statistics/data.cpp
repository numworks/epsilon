#include "data.h"
#include <assert.h>
#include <float.h>

namespace Statistics {

Data::Data() :
  m_numberOfPairs(0),
  m_binWidth(1.0f),
  m_totalSize(0),
  m_selectedBin(0.0f),
  m_minValue(0.0f),
  m_maxValue(0.0f),
  m_xMin(0.0f),
  m_xMax(10.0f),
  m_yMax(1.0f),
  m_xGridUnit(1.0f)
{
}

int Data::numberOfPairs() const {
  return m_numberOfPairs;
}

float Data::binWidth() {
  return m_binWidth;
}

void Data::setBinWidth(float binWidth) {
  m_binWidth = binWidth;
  computeYMax();
  initSelectedBin();
}

float Data::valueAtIndex(int index) {
  assert(index < m_numberOfPairs);
  return m_values[index];
}

int Data::sizeAtIndex(int index) {
  assert(index < m_numberOfPairs);
  return m_sizes[index];
}

void Data::setValueAtIndex(float value, int index) {
  if (index >= k_maxNumberOfPairs) {
    return;
  }
  float formerValue =  index >= m_numberOfPairs ? value : m_values[index];
  m_values[index] = value;
  if (index >= m_numberOfPairs) {
    m_sizes[index] = 1;
    m_totalSize += 1;
    m_numberOfPairs++;
  }
  updateAbsoluteBoundsAfterDeleting(formerValue);
  updateAbsoluteBoundsAfterAdding(value);
  initSelectedBin();
}

void Data::setSizeAtIndex(int size, int index) {
  if (index >= k_maxNumberOfPairs) {
    return;
  }
  float formerValue =  index >= m_numberOfPairs ? 0.0f : m_values[index];
  int formerSize =  index >= m_numberOfPairs ? 0 : m_sizes[index];
  m_sizes[index] = size;
  m_totalSize += size - formerSize;
  if (index >= m_numberOfPairs) {
    m_values[index] = 0.0f;
    m_numberOfPairs++;
  }
  updateAbsoluteBoundsAfterDeleting(formerValue);
  updateAbsoluteBoundsAfterAdding(m_values[index]);
  initSelectedBin();
}

void Data::deletePairAtIndex(int index) {
  m_numberOfPairs--;
  float formerValue = m_values[index];
  m_totalSize -= m_sizes[index];
  for (int k = index; k < m_numberOfPairs; k++) {
    m_values[k] = m_values[k+1];
    m_sizes[k] = m_sizes[k+1];
  }
  m_values[m_numberOfPairs] = 0.0f;
  m_sizes[m_numberOfPairs] = 0;
  if (m_minValue == formerValue) {
    computeAbsoluteBoundValue();
  }
  updateAbsoluteBoundsAfterDeleting(formerValue);
  initSelectedBin();
}

int Data::sizeAtValue(float value) {
  if (value < m_minValue) {
    return 0;
  }
  float bin = binWidth();
  int binNumber = (value - m_minValue)/bin;
  float lowerBound = m_minValue + binNumber*bin;
  float upperBound = m_minValue + (binNumber+1)*bin;
  int result = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_values[k] < upperBound && lowerBound <= m_values[k]) {
      result += m_sizes[k];
    }
  }
  return result;
}

int Data::totalSize() {
  return m_totalSize;
}

float Data::minValue() {
  return m_minValue;
}

void Data::setMinValue(float minValue) {
  m_minValue = minValue;
  computeTotalSize();
  computeYMax();
  initSelectedBin();
}

float Data::selectedBin() {
  return m_selectedBin;
}

void Data::initSelectedBin() {
  m_selectedBin = m_minValue + m_binWidth/2;
  while (sizeAtValue(m_selectedBin) == 0) {
    m_selectedBin += m_binWidth;
  }
  initWindowBounds();
}

void Data::selectNextBinToward(int direction) {
  float newSelectedBin = m_selectedBin;
  if (direction > 0.0f) {
    do {
      newSelectedBin += m_binWidth;
    } while (sizeAtValue(newSelectedBin) == 0 && newSelectedBin < m_maxValue);
  }
  if (direction < 0.0f) {
    do {
      newSelectedBin -= m_binWidth;
    } while (sizeAtValue(newSelectedBin) == 0 && newSelectedBin > m_minValue);
  }
  if (newSelectedBin > m_minValue && newSelectedBin < m_maxValue + m_binWidth) {
    m_selectedBin = newSelectedBin;
    scrollToSelectedBin();
  }
}

float Data::xMin() {
  return m_xMin;
}

float Data::xMax() {
  return m_xMax;
}

float Data::yMin() {
  return -k_marginFactor*m_yMax;
}

float Data::yMax() {
  return m_yMax;
}

float Data::xGridUnit() {
  return m_xGridUnit;
}

void Data::computeTotalSize() {
  m_totalSize = 0;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_values[k] >= m_minValue) {
      m_totalSize += m_sizes[k];
    }
  }
}

void Data::scrollToSelectedBin() {
  float range = m_xMax - m_xMin;
  if (m_xMin > m_selectedBin) {
    m_xMin = m_selectedBin - m_binWidth/2;
    m_xMax = m_xMin + range;
  }
  if (m_selectedBin > m_xMax) {
    m_xMax = m_selectedBin + m_binWidth/2;
    m_xMin = m_xMax - range;
  }
}

void Data::initWindowBounds() {
  m_xMin = m_minValue;
  if (m_maxValue - m_xMin > k_maxHistogramRangeValue) {
    m_xMax = m_xMin + 10.0f;
  } else {
    m_xMax = m_maxValue + m_binWidth;
  }
  m_xGridUnit = computeGridUnit(Axis::X);
}

void Data::computeYMax() {
  m_yMax = -FLT_MAX;
  for (float x = m_minValue; x <= m_maxValue; x += m_binWidth) {
    float size = sizeAtValue(x);
    if (size > m_yMax) {
      m_yMax = size;
    }
  }
  m_yMax = m_yMax/m_totalSize;
}

void Data::computeAbsoluteBoundValue() {
  m_minValue = FLT_MAX;
  m_maxValue = -FLT_MAX;
  for (int k = 0; k < m_numberOfPairs; k++) {
    if (m_values[k] < m_minValue) {
      m_minValue = m_values[k];
    }
    if (m_values[k] > m_maxValue) {
      m_maxValue = m_values[k];
    }
  }
  m_binWidth = 1.0f;
  initWindowBounds();
  computeYMax();
}

void Data::updateAbsoluteBoundsAfterAdding(float value) {
  if (m_minValue > value) {
    m_minValue = value;
  }
  if (m_maxValue < value) {
    m_maxValue = value;
  }
  m_binWidth = 1.0f;
  initWindowBounds();
  computeYMax();
}

void Data::updateAbsoluteBoundsAfterDeleting(float value) {
  if (value == m_minValue || value == m_maxValue) {
    computeAbsoluteBoundValue();
  }
}

}
