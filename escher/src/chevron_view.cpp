#include <escher/chevron_view.h>
#include <escher/palette.h>

const KDColor chevronPixel[] = {
  KDColorBlack, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorWhite, KDColorWhite, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorWhite, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorWhite, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorWhite, KDColorWhite, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite,
  KDColorBlack, KDColorBlack, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite,
  KDColorBlack, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite, KDColorWhite,
};

const KDColor highlightedChevronPixel[] = {
  KDColorBlack, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select,
  KDColorBlack, KDColorBlack, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select,
  KDColorBlack, KDColorBlack, KDColorBlack, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::Select, Palette::Select, Palette::Select, Palette::Select,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::Select, Palette::Select, Palette::Select,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::Select, Palette::Select,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::Select,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::Select,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::Select, Palette::Select,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::Select, Palette::Select, Palette::Select,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::Select, Palette::Select, Palette::Select, Palette::Select,
  KDColorBlack, KDColorBlack, KDColorBlack, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select,
  KDColorBlack, KDColorBlack, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select,
  KDColorBlack, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select, Palette::Select,
};

ChevronView::ChevronView() :
  m_highlighted(false)
{
}

void ChevronView::setHighlighted(bool highlight) {
  m_highlighted = highlight;
  markRectAsDirty(bounds());
}

void ChevronView::drawRect(KDContext * ctx, KDRect rect) const {
  /* Draw the chevron aligned on the right of the view and vertically centered.
   * The heightCenter is the coordinate of the vertical middle of the view. That
   * way, (heightCenter-switchHalfHeight) indicates the top the chevron. */
  KDCoordinate width = bounds().width();
  KDCoordinate heightCenter =  bounds().height()/2;
  KDCoordinate chevronHalfHeight = k_chevronHeight/2;
  KDRect frame(width - k_chevronRightMargin, heightCenter -chevronHalfHeight, k_chevronWidth, k_chevronHeight);
  if (m_highlighted) {
    ctx->fillRectWithPixels(frame, highlightedChevronPixel, (KDColor *)m_workingBuffer);
  } else {
    ctx->fillRectWithPixels(frame, chevronPixel, (KDColor *)m_workingBuffer);
  }
}

KDSize ChevronView::minimalSizeForOptimalDisplay() {
  return KDSize(k_chevronRightMargin, k_chevronHeight);
}
