#ifndef REGRESSION_RESIDUAL_PLOT_BANNER_VIEW_H
#define REGRESSION_RESIDUAL_PLOT_BANNER_VIEW_H

#include <apps/shared/banner_view.h>
#include <escher/buffer_text_view.h>
#include <escher/responder.h>
#include <poincare/print_float.h>

namespace Regression {

class ResidualPlotBannerView : public Shared::BannerView {
 public:
  ResidualPlotBannerView()
      : m_abscissaView(k_bannerFieldFormat),
        m_ordinateView(k_bannerFieldFormat),
        m_stddevView(k_bannerFieldFormat) {}
  Escher::AbstractBufferTextView* abscissaView() { return &m_abscissaView; }
  Escher::AbstractBufferTextView* ordinateView() { return &m_ordinateView; }
  Escher::AbstractBufferTextView* stddevView() { return &m_stddevView; }
  constexpr static int k_numberOfSubviews = 3;

 private:
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  View* subviewAtIndex(int index) override {
    return index == 0 ? abscissaView()
                      : (index == 1 ? ordinateView() : stddevView());
  }

  BannerBufferTextView m_abscissaView;
  BannerBufferTextView m_ordinateView;
  BannerBufferTextView m_stddevView;
};

}  // namespace Regression

#endif
