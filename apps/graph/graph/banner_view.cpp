#include "banner_view.h"

#include <apps/i18n.h>
#include <assert.h>

using namespace Escher;

namespace Graph {

BannerView::BannerView(Responder* parentResponder,
                       TextFieldDelegate* textFieldDelegate)
    : Shared::XYBannerView(parentResponder, textFieldDelegate),
      m_firstDerivativeView(k_bannerFieldFormat),
      m_secondDerivativeView(k_bannerFieldFormat),
      m_slopeView(k_bannerFieldFormat),
      m_tangentEquationView(I18n::Message::LinearRegressionFormula,
                            k_bannerFieldFormat),
      m_aView(k_bannerFieldFormat),
      m_bView(k_bannerFieldFormat) {
  for (int i = 0; i < k_maxNumberOfInterests; i++) {
    m_interestMessageView[i] =
        MessageTextView(I18n::Message::Default, k_bannerFieldFormat);
  }
}

void BannerView::addInterestMessage(I18n::Message message) {
  int n = numberOfInterestMessages();
  if (n == k_maxNumberOfInterests) {
    return;
  }
  m_interestMessageView[n].setMessage(message);
}

void BannerView::emptyInterestMessages() {
  for (int i = 0; i < k_maxNumberOfInterests; i++) {
    m_interestMessageView[i].setMessage(I18n::Message::Default);
  }
}

void BannerView::setDisplayParameters(DisplayParameters displayParameters) {
  m_displayParameters = displayParameters;
}

View* BannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  // - Interest messages
  int n = numberOfInterestMessages();
  if (index < n) {
    return m_interestMessageView + index;
  }
  index -= n;
  // - Abscissa
  if (index == 0) {
    return editableView();
  }
  index--;
  // - Ordinate
  if (m_displayParameters.showOrdinate && index == 0) {
    return &m_ordinateView;
  }
  index -= m_displayParameters.showOrdinate;
  // - First derivative
  if (m_displayParameters.showFirstDerivative && index == 0) {
    return &m_firstDerivativeView;
  }
  index -= m_displayParameters.showFirstDerivative;
  // - Second derivative
  if (m_displayParameters.showSecondDerivative && index == 0) {
    return &m_secondDerivativeView;
  }
  index -= m_displayParameters.showSecondDerivative;
  // - Slope (dx/dy)
  if (m_displayParameters.showSlope && index == 0) {
    return &m_slopeView;
  }
  index -= m_displayParameters.showSlope;
  // - Tangent subviews
  assert(m_displayParameters.showTangent);
  assert(0 <= index && index < 3);
  View* subviews[] = {&m_tangentEquationView, &m_aView, &m_bView};
  return subviews[index];
}

bool BannerView::lineBreakBeforeSubview(View* subview) const {
  return subview == &m_tangentEquationView ||
         Shared::XYBannerView::lineBreakBeforeSubview(subview);
}

int BannerView::numberOfInterestMessages() const {
  int result = 0;
  for (int i = 0; i < k_maxNumberOfInterests; i++) {
    if (hasInterestMessage(i)) {
      assert(result == i);  // Interests messages should be filled in order
      result++;
    }
  }
  return result;
}

}  // namespace Graph
