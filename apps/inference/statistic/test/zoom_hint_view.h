#ifndef INFERENCE_STATISTIC_TEST_HINT_VIEW_H
#define INFERENCE_STATISTIC_TEST_HINT_VIEW_H

#include <apps/i18n.h>
#include <apps/shared/banner_view.h>
#include <escher/key_view.h>
#include <escher/message_text_view.h>
#include <escher/view.h>

namespace Inference {

class ZoomHintView : public Escher::View {
 public:
  constexpr static KDFont::Size k_legendFont = KDFont::Size::Small;
  constexpr static KDCoordinate k_legendHeight =
      2 * Escher::Metric::BannerTextMargin + KDFont::GlyphHeight(k_legendFont);

  ZoomHintView();
  void drawRect(KDContext* ctx, KDRect rect) const override;

 private:
  constexpr static int k_numberOfLegends = 2;
  constexpr static int k_numberOfTokens = 2;
  constexpr static KDCoordinate k_tokenWidth = 10;
  constexpr static KDColor k_backgroundColor =
      Shared::BannerView::k_bannerFieldFormat.style.backgroundColor;

  void layoutSubviews(bool force = false) override;
  int numberOfSubviews() const override;
  Escher::View* subviewAtIndex(int index) override;
  Escher::MessageTextView m_legends[k_numberOfLegends];
  Escher::KeyView m_legendPictograms[k_numberOfTokens];
};

}  // namespace Inference

#endif
