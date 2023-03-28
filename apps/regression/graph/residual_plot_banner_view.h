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
      : m_abscissaView(k_defaultFormat),
        m_ordinateView(k_defaultFormat),
        m_stddevView(k_defaultFormat) {}
  Escher::BufferTextView* abscissaView() { return &m_abscissaView; }
  Escher::BufferTextView* ordinateView() { return &m_ordinateView; }
  Escher::BufferTextView* stddevView() { return &m_stddevView; }
  constexpr static int k_numberOfSubviews = 3;

 private:
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  View* subviewAtIndex(int index) override {
    return index == 0 ? abscissaView()
                      : (index == 1 ? ordinateView() : stddevView());
  }

  Escher::BufferTextView m_abscissaView;
  Escher::BufferTextView m_ordinateView;
  Escher::BufferTextView m_stddevView;
};

}  // namespace Regression

#endif
