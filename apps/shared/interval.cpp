#include <assert.h>
#include <cmath>
#include <poincare/print_float.h>
#include "interval.h"
#include "poincare_helpers.h"

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

bool Interval::hasSameParameters(IntervalParameters parameters) {
  return (m_parameters.start() == parameters.start() && m_parameters.end() == parameters.end() && m_parameters.step() == parameters.step());
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

void Interval::translateTo(double newStart) {
  double previousStart = m_parameters.start();
  m_parameters.setStart(newStart);
  m_parameters.setEnd(m_parameters.end() + newStart - previousStart);
  m_parameters.setStep(1.0);
  m_needCompute = true;
}

void Interval::computeElements() {
  if (!m_needCompute) {
    return;
  }
  if (isEmpty()) {
    m_numberOfElements = 0;
  } else {
    m_numberOfElements = m_parameters.step() > 0 ? 1 + (m_parameters.end() - m_parameters.start())/m_parameters.step() : k_maxNumberOfElements;
    m_numberOfElements = m_numberOfElements > k_maxNumberOfElements || m_numberOfElements < 0 ? k_maxNumberOfElements : m_numberOfElements;
  }
  /* Even though elements are displayed with 7 significant digits, we round the
   * element to 14 significant digits to prevent unexpected imprecisions due to
   * doubles. For example, with start=-0.2 and step=0.2, 6th element is
   * 1.0000000000000002 although 1.0 was expected. */
  constexpr int precision = Poincare::PrintFloat::k_numberOfStoredSignificantDigits;
  static_assert(precision == 14, "ratioThreshold value should be updated");
  // Save some calls to std::pow(10.0, -precision)
  constexpr double ratioThreshold = 10e-14;
  bool checkForElementZero = (m_parameters.start() < 0.0);
  for (int i = 0; i < m_numberOfElements; i += 1) {
    if (checkForElementZero && i > 0 && std::abs(1.0 + m_parameters.start() / (i * m_parameters.step())) < ratioThreshold) {
      /* We also round to 0 if start/(i*step) would have been rounded to -1.
       * For example, with start=-1.2, and step=0.2, 6th element should be 0
       * instead of 2.22e-16. */
      m_intervalBuffer[i] = 0.0;
    } else {
      m_intervalBuffer[i] = PoincareHelpers::ValueOfFloatAsDisplayed<double>(m_parameters.start() + i * m_parameters.step(), precision, nullptr);
      checkForElementZero = checkForElementZero && m_intervalBuffer[i] < 0.0;
    }
  }
  m_needCompute = false;
}

}
