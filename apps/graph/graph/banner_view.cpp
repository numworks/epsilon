#include "banner_view.h"
#include <assert.h>
#include <string.h>
#include "../../constant.h"

namespace Graph {

BannerView::BannerView(GraphWindow * graphWindow) :
  m_abscissaView(0.0f, 0.5f),
  m_functionView(0.5f, 0.5f),
  m_derivativeView(1.0f, 0.5f),
  m_displayDerivative(false),
  m_graphWindow(graphWindow)
{
}

void BannerView::reload() {
  markRectAsDirty(bounds());
  char buffer[k_maxNumberOfCharacters+Constant::FloatBufferSizeInScientificMode] = {'x', ' ', '=', ' ',0};
  Float(m_graphWindow->xCursorPosition()).convertFloatToText(buffer+4, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_abscissaView.setText(buffer);

  strlcpy(buffer, "00(x) = ", k_maxNumberOfCharacters+1);
  buffer[1] = m_graphWindow->functionSelectedByCursor()->name()[0];
  Float(m_graphWindow->yCursorPosition()).convertFloatToText(buffer+8, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_functionView.setText(buffer+1);

  if (m_displayDerivative) {
    buffer[0] = m_graphWindow->functionSelectedByCursor()->name()[0];
    buffer[1] = '\'';
    Float(m_graphWindow->derivativeAtCursorPosition()).convertFloatToText(buffer+8, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaForDerivativeNumberInScientificMode);
    m_derivativeView.setText(buffer);
  }
}

void BannerView::setDisplayDerivative(bool displayDerivative) {
  m_displayDerivative = displayDerivative;
}

bool BannerView::displayDerivative() {
  return m_displayDerivative;
}

int BannerView::numberOfSubviews() const {
  if (m_displayDerivative) {
    return 3;
  }
  return 2;
}

View * BannerView::subviewAtIndex(int index) {
  assert(index >= 0);
  if (index == 0) {
    return &m_abscissaView;
  }
  if (index == 1) {
    return &m_functionView;
  }
  return &m_derivativeView;
}

void BannerView::layoutSubviews() {
  m_abscissaView.setFrame(bounds());
  m_functionView.setFrame(bounds());
  m_derivativeView.setFrame(bounds());
}

}
