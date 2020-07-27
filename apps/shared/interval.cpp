#include "interval.h"
#include <assert.h>

namespace Shared {

Interval::Interval() :
  m_numberOfElements(0)
{
  reset();
}

int Interval::numberOfElements() {
  computeElements();
  return m_numberOfElements;
}

void Interval::deleteElementAtIndex(int index) {
  assert(!m_needCompute);
  assert(m_numberOfElements > 0);
  for (int k = index; k < m_numberOfElements-1; k++) {
    m_intervalBuffer[k] = m_intervalBuffer[k+1];
  }
  m_numberOfElements--;
}

double Interval::element(int i) {
  assert(i >= 0 && i < numberOfElements());
  computeElements();
  return m_intervalBuffer[i];
}

void Interval::setElement(int i, double f) {
  assert(i <= numberOfElements() && i < k_maxNumberOfElements);
  computeElements();
  m_intervalBuffer[i] = f;
  if (i == numberOfElements()) {
    m_numberOfElements++;
  }
}

void Interval::reset() {
  m_parameters.setStart(0.0);
  m_parameters.setEnd(10.0);
  m_parameters.setStep(1.0);
  m_needCompute = true;
}

void Interval::clear() {
  m_parameters.setStart(1.0);
  m_parameters.setEnd(0.0);
  m_parameters.setStep(1.0);
  m_needCompute = true;
}

void Interval::computeElements() {
  if (!m_needCompute) {
    return;
  }
  if (m_parameters.start() > m_parameters.end()) {
    m_numberOfElements = 0;
  } else {
    m_numberOfElements = m_parameters.step() > 0 ? 1 + (m_parameters.end() - m_parameters.start())/m_parameters.step() : k_maxNumberOfElements;
    m_numberOfElements = m_numberOfElements > k_maxNumberOfElements || m_numberOfElements < 0 ? k_maxNumberOfElements : m_numberOfElements;
  }
  for (int i = 0; i < m_numberOfElements; i += 1) {
    m_intervalBuffer[i] = m_parameters.start() + i * m_parameters.step();
  }
  m_needCompute = false;
}

}
