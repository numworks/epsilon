#ifndef INFERENCE_STATISTIC_TEST_LEGEND_VIEW_H
#define INFERENCE_STATISTIC_TEST_LEGEND_VIEW_H

#include <apps/i18n.h>
#include <escher/message_text_view.h>
#include <escher/view.h>

namespace Inference {

class LegendView : public Escher::View {
public:
  LegendView();

  int numberOfSubviews() const override { return 4; }
  Escher::View * subviewAtIndex(int i) override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force) override;

private:
  constexpr static int k_marginBetween = 5;
  constexpr static int k_offsetTop = 3;
  constexpr static int k_diameter = 8;

  class DotView : public Escher::View {
  public:
    DotView(KDColor color) : m_color(color) {}

    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override { return KDSize(k_diameter, k_diameter); }

  private:
    KDColor m_color;
  };

  Escher::MessageTextView m_pValueLabel;
  Escher::MessageTextView m_alphaLabel;
  DotView m_pValueIcon;
  DotView m_alphaIcon;
};

}  // namespace Inference

#endif /* INFERENCE_STATISTIC_TEST_LEGEND_VIEW_H */
