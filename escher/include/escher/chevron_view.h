#ifndef ESCHER_CHEVRON_VIEW_H
#define ESCHER_CHEVRON_VIEW_H

#include <escher/view.h>

class ChevronView : public View {
public:
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setHighlighted(bool highlight);
private:
  /* k_chevronHeight and k_chevronWidth are the dimensions of the chevron. */
  constexpr static KDCoordinate k_chevronHeight = 16;
  constexpr static KDCoordinate k_chevronWidth = 8;
  constexpr static KDCoordinate k_chevronRightMargin = 20;
  bool m_highlighted;
  KDColor m_workingBuffer[k_chevronWidth*k_chevronHeight];
};

#endif
