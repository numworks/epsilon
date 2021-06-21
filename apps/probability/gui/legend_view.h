#ifndef APPS_PROBABILITY_GUI_LEGEND_VIEW_H
#define APPS_PROBABILITY_GUI_LEGEND_VIEW_H

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view.h>

#include "horizontal_or_vertical_layout.h"

namespace Probability {

class LegendLabel : public HorizontalLayout {
public:
  LegendLabel(const char * label, KDColor color) : m_icon(color) {
    m_labelView.setText(label);
    m_labelView.setBackgroundColor(Escher::Palette::WallScreen);
    m_labelView.setFont(KDFont::SmallFont);
  }
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
  class Icon : public Escher::View {
  public:
    Icon(KDColor color) : m_color(color) {}
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override { return KDSize(k_diameter, k_diameter); }

  private:
    KDColor m_color;
  };
  Escher::BufferTextView m_labelView;
  Icon m_icon;
};

class LegendView : public VerticalLayout {
public:
  LegendView() :
      m_pValue(I18n::translate(I18n::Message::PValue), KDColorOrange),
      m_alpha(I18n::translate(I18n::Message::Alpha), Palette::GrayMiddle) {}
  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int i) override { return i == 0 ? &m_pValue : &m_alpha; }

private:
  LegendLabel m_pValue;
  LegendLabel m_alpha;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_LEGEND_VIEW_H */
