#include "banner_view.h"
#include <assert.h>

using namespace Escher;

namespace Regression {

BannerView::BannerView(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate) :
  Shared::XYBannerView(parentResponder, inputEventHandlerDelegate, textFieldDelegate),
  m_otherView(Font(), KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()),
  m_otherViewIsFirst(false),
  m_numberOfSubviews(k_maxNumberOfSubviews)
{}

void BannerView::setDisplayParameters(bool hideOtherView, bool otherViewIsFirst) {
  assert(!hideOtherView || !otherViewIsFirst);
  m_otherViewIsFirst = otherViewIsFirst && !hideOtherView;
  m_numberOfSubviews = k_maxNumberOfSubviews - hideOtherView;
}

View * BannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < m_numberOfSubviews);
  if (m_otherViewIsFirst) {
    if (index == 0) {
      return &m_otherView;
    }
    index--;
  } else if (index == k_maxNumberOfSubviews - 1) {
    return &m_otherView;
  }
  return Shared::XYBannerView::subviewAtIndex(index);
}

}
