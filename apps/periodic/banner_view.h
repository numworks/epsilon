#ifndef PERIODIC_BANNER_VIEW_H
#define PERIODIC_BANNER_VIEW_H

#include "elements_view_data_source.h"
#include <escher/buffer_text_view.h>
#include <escher/palette.h>
#include <escher/view.h>
#include <ion/display.h>

namespace Periodic {

class BannerView : public Escher::View {
public:
  BannerView(ElementsViewDataSource * dataSource) : m_textView(KDFont::Size::Small, KDContext::k_alignLeft, KDContext::k_alignCenter, k_legendColor, k_backgroundColor), m_dataSource(dataSource) {}

  // Escher::View
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override { return KDSize(Ion::Display::Width, k_bannerHeight + k_borderHeight); }

  void reload() { layoutSubviews(); }

private:
  constexpr static KDColor k_legendColor = Escher::Palette::GrayVeryDark;
  constexpr static KDColor k_backgroundColor = Palette::SystemGreyLight;
  constexpr static KDCoordinate k_dotLeftMargin = 16;
  constexpr static KDCoordinate k_dotDiameter = 8;
  constexpr static KDCoordinate k_bannerHeight = 25;
  constexpr static KDCoordinate k_borderHeight = 1;
  constexpr static KDCoordinate k_dotLegendMargin = 12;

  class DotView : public Escher::View {
  public:
    DotView() : m_color(k_backgroundColor) {}

    // Escher::View
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override { return KDSize(k_dotDiameter, k_dotDiameter); }

    void setColor(KDColor color) { m_color = color; }

  private:
    KDColor m_color;
  };

  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  DotView m_dotView;
  Escher::BufferTextView m_textView;
  ElementsViewDataSource * m_dataSource;
};

}

#endif
