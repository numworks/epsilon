#include "store.h"

Store::Store() :
  m_currentIndex(0)
{
  for (int i = 0; i < k_maxNumberOfStoredValues; i++) {
    m_storedValues[i] = 0;
  }
}

void Store::push(int number) {
  m_storedValues[m_currentIndex] = number;
  m_currentIndex = (m_currentIndex + 1) % k_maxNumberOfStoredValues;
}

int Store::add(int number) {
  return operate(number, true);
}

int Store::subtract(int number) {
  return operate(number, false);
}

const int Store::value(int index) const {
  return m_storedValues[(m_currentIndex + index) % k_maxNumberOfStoredValues];
}

int Store::operate(int number, bool add) {
  int lastIndex = (m_currentIndex + k_maxNumberOfStoredValues - 1) % k_maxNumberOfStoredValues;
  int result = m_storedValues[lastIndex] + (add ? number : -number);
  m_storedValues[lastIndex] = 0;
  m_currentIndex = lastIndex;
  return result;
}
