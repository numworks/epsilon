#include "banner_view.h"

#include <apps/i18n.h>
#include <assert.h>
#include <kandinsky/font.h>

using namespace Escher;

namespace Regression {

BannerView::BannerView(Responder* parentResponder,
                       InputEventHandlerDelegate* inputEventHandlerDelegate,
                       TextFieldDelegate* textFieldDelegate)
    : Shared::XYBannerView(parentResponder, inputEventHandlerDelegate,
                           textFieldDelegate),
      m_otherView(k_bannerFieldFormat),
      m_dataNotSuitableView(I18n::Message::DataNotSuitableForRegression,
                            k_bannerFieldFormat),
      m_displayOtherView(false),
      m_otherViewIsFirst(false),
      m_displayDataNotSuitable(false) {}

void BannerView::setDisplayParameters(bool displayOtherView,
                                      bool otherViewIsFirst,
                                      bool displayDataNotSuitable) {
  assert(displayOtherView || !otherViewIsFirst);
  m_displayOtherView = displayOtherView;
  m_otherViewIsFirst = otherViewIsFirst;
  m_displayDataNotSuitable = displayDataNotSuitable;
}

View* BannerView::subviewAtIndex(int index) {
  // In the subviews order :
  assert(0 <= index && index < numberOfSubviews());
  assert(m_displayOtherView || !m_otherViewIsFirst);
  // - OtherView if first and displayed
  if (m_otherViewIsFirst && index == 0) {
    return &m_otherView;
  }
  index -= m_otherViewIsFirst;
  // - XYBanner subviews
  if (index < XYBannerView::k_numberOfSubviews) {
    return XYBannerView::subviewAtIndex(index);
  }
  // - OtherView if not first and displayed
  if (m_displayOtherView && !m_otherViewIsFirst &&
      index == XYBannerView::k_numberOfSubviews) {
    return &m_otherView;
  }
  // - DataNotSuitable if displayed
  assert(m_displayDataNotSuitable &&
         index + m_otherViewIsFirst == numberOfSubviews() - 1);
  return &m_dataNotSuitableView;
}

}  // namespace Regression
