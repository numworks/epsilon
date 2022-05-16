#ifndef SHARED_BANNER_VIEW_H
#define SHARED_BANNER_VIEW_H

#include <escher/metric.h>
#include <escher/view.h>
#include <escher/palette.h>

namespace Shared {

class BannerView : public Escher::View {
public:
  // Ion::Display::Width / KDFont::SmallFont->glyphSize().width()
  constexpr static float k_maxLengthDisplayed = 45;

  static KDCoordinate HeightGivenNumberOfLines(int linesCount);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  KDCoordinate minimalHeightForOptimalDisplayGivenWidth(KDCoordinate width) const;
  void reload() { layoutSubviews(); }
  static constexpr const KDFont * Font() { return KDFont::SmallFont; }
  static constexpr KDColor TextColor() { return KDColorBlack; }
  static constexpr KDColor BackgroundColor() { return Escher::Palette::GrayMiddle; }

protected:
  class LabelledView : public Escher::View {
  public:
    LabelledView(Escher::View * labelView, Escher::View * infoView) : m_labelView(labelView), m_infoView(infoView) {}
    KDSize minimalSizeForOptimalDisplay() const override;

  private:
    int numberOfSubviews() const override { return 2; }
    Escher::View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;

    Escher::View * m_labelView;
    Escher::View * m_infoView;
  };

  virtual bool lineBreakBeforeSubview(Escher::View * subview) const { return false; }

private:
  static constexpr KDCoordinate LineSpacing = Escher::Metric::BannerTextMargin;
  static constexpr KDCoordinate k_minimalSpaceBetweenSubviews = 14; // Width of '  ' in SmallFont
  int numberOfSubviews() const override = 0;
  View * subviewAtIndex(int index) override = 0;
  void layoutSubviews(bool force = false) override;
  int numberOfLinesGivenWidth(KDCoordinate width) const;
  int numberOfSubviewsOnOneLine(int firstSubview, KDCoordinate width, KDCoordinate * remainingWidth = nullptr) const;
};

}

#endif
