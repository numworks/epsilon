#ifndef REGRESSION_XY_BANNER_VIEW_H
#define REGRESSION_XY_BANNER_VIEW_H

#include <escher/buffer_text_view.h>
#include <apps/shared/banner_view.h>

namespace Regression {

class XYBannerView : public Shared::BannerView {
public:
  static constexpr int k_numberOfSubviews = 2;

  XYBannerView() :
    m_abscissaView(Font(), KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor()),
    m_ordinateView(Font(), KDContext::k_alignCenter, KDContext::k_alignCenter, TextColor(), BackgroundColor())
    {}
  Escher::BufferTextView * abscissaView() { return &m_abscissaView; }
  Escher::BufferTextView * ordinateView() { return &m_ordinateView; }

  Escher::View * subviewAtIndex(int index) override { return index == 0 ? &m_abscissaView : &m_ordinateView; }
private:
  int numberOfSubviews() const override { return k_numberOfSubviews; }

  Escher::BufferTextView m_abscissaView;
  Escher::BufferTextView m_ordinateView;
};

}

#endif
