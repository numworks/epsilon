#ifndef PROBABILITY_GUI_LEGEND_VIEW_H
#define PROBABILITY_GUI_LEGEND_VIEW_H

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/view.h>

namespace Inference {

/* A View displaying the p-value legend for Test graphs */
class LegendView : public Escher::View {
public:
  LegendView(const char * label = I18n::translate(I18n::Message::PValue),
             KDColor color = KDColorOrange);
  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int i) override {
    View * subviews[] = {&m_icon, &m_labelView};
    return subviews[i];
  }
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

  Escher::BufferTextView m_labelView;
  DotView m_icon;
};

}  // namespace Inference

#endif /* PROBABILITY_GUI_LEGEND_VIEW_H */
