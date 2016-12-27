#include "data.h"

namespace Statistics {

Data::Data() :
  m_numberOfPairs(0)
{
}

int Data::numberOfPairs() const {
  return m_numberOfPairs;
}

float Data::valueAtIndex(int index) {
  return m_values[index];
}

int Data::sizeAtIndex(int index) {
  return m_sizes[index];
}

void Data::setValueAtIndex(float value, int index) {
  m_values[index] = value;
  if (index >= m_numberOfPairs) {
    m_sizes[index] = 0;
    m_numberOfPairs++;
  }
}

void Data::setSizeAtIndex(int size, int index) {
  m_sizes[index] = size;
  if (index >= m_numberOfPairs) {
    m_values[index] = 0.0f;
    m_numberOfPairs++;
  }
}

}
