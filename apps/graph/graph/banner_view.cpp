#include "banner_view.h"
#include <assert.h>
#include "../../constant.h"

namespace Graph {

BannerView::BannerView() :
  m_abscissa(0.0f),
  m_function(nullptr),
  m_abscissaView(0.0f, 0.5f),
  m_functionView(0.5f, 0.5f),
  m_derivativeView(1.0f, 0.5f),
  m_displayDerivative(false),
  m_context(nullptr)
{
}

void BannerView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

void BannerView::setContext(Context * context) {
  m_context = context;
}

void BannerView::setAbscissa(float x) {
  markRectAsDirty(bounds());
  m_abscissa = x;
  char buffer[4+Constant::FloatBufferSizeInScientificMode] = {'x', ' ', '=', ' ',0};
  Float(x).convertFloatToText(buffer+4, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_abscissaView.setText(buffer);
}

void BannerView::setFunction(Function * f) {
  markRectAsDirty(bounds());
  m_function = f;
  float y = f->evaluateAtAbscissa(m_abscissa, m_context);
  char buffer[8+Constant::FloatBufferSizeInScientificMode] = {0, 0, '(', 'x', ')', ' ', '=', ' ',0};
  buffer[1] = f->name()[0];
  Float(y).convertFloatToText(buffer+8, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_functionView.setText(buffer+1);

  y = f->approximateDerivative(m_abscissa, m_context);
  buffer[0] = f->name()[0];
  buffer[1] = '\'';
  Float(y).convertFloatToText(buffer+8, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaForDerivativeNumberInScientificMode);
  m_derivativeView.setText(buffer);
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
