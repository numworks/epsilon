#include <escher/chevron_view.h>
#include <escher/palette.h>

const KDColor chevronPixel[] = {
  KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
  KDColorBlack, KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
  KDColorBlack, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor, Palette::CellBackgroundColor,
};

const KDColor highlightedChevronPixel[] = {
  KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
  KDColorBlack, KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
  KDColorBlack, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor, Palette::FocusCellBackgroundColor,
};

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
