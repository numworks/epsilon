#ifndef SHARED_BANNER_VIEW_H
#define SHARED_BANNER_VIEW_H

#include <escher/metric.h>
#include <escher/palette.h>
#include <escher/view.h>
#include <ion.h>

namespace Shared {

class BannerView : public Escher::View {
 public:
  constexpr static KDFont::Size k_font = KDFont::Size::Small;
  constexpr static KDColor TextColor() { return KDColorBlack; }
  constexpr static KDColor BackgroundColor() {
    return Escher::Palette::GrayMiddle;
  }
  constexpr static float k_maxLengthDisplayed =
      Ion::Display::Width / KDFont::GlyphWidth(k_font);

  static KDCoordinate HeightGivenNumberOfLines(int linesCount);
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  KDCoordinate minimalHeightForOptimalDisplayGivenWidth(
      KDCoordinate width) const;
  void reload() { layoutSubviews(); }

  class LabelledView : public Escher::View {
   public:
    LabelledView(Escher::View* labelView, Escher::View* infoView)
        : m_labelView(labelView), m_infoView(infoView) {}
    KDSize minimalSizeForOptimalDisplay() const override;

   private:
    int numberOfSubviews() const override { return 2; }
    Escher::View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;

    Escher::View* m_labelView;
    Escher::View* m_infoView;
  };

 protected:
  virtual bool lineBreakBeforeSubview(Escher::View* subview) const {
    return false;
  }

 private:
  constexpr static KDCoordinate LineSpacing = Escher::Metric::BannerTextMargin;
  // Width of '  '
  constexpr static KDCoordinate k_minimalSpaceBetweenSubviews =
      2 * KDFont::GlyphWidth(k_font);
  int numberOfSubviews() const override = 0;
  View* subviewAtIndex(int index) override = 0;
  void layoutSubviews(bool force = false) override;
  int numberOfLinesGivenWidth(KDCoordinate width) const;
  int numberOfSubviewsOnOneLine(int firstSubview, KDCoordinate width,
                                KDCoordinate* remainingWidth = nullptr) const;
};

}  // namespace Shared

#endif
