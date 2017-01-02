#include "curve_view_with_banner_and_cursor.h"
#include <assert.h>

CurveViewWithBannerAndCursor::CurveViewWithBannerAndCursor(CurveViewWindowWithCursor * curveViewWindowWithCursor, float topMarginFactor,
    float rightMarginFactor, float bottomMarginFactor, float leftMarginFactor) :
  CurveViewWithBanner(curveViewWindowWithCursor, topMarginFactor, rightMarginFactor, bottomMarginFactor, leftMarginFactor),
  m_cursorView(CursorView()),
  m_curveViewWindowWithCursor(curveViewWindowWithCursor)
{
}

void CurveViewWithBannerAndCursor::reload() {
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
  computeLabels(Axis::Vertical);
  layoutSubviews();
  bannerView()->reload();
}

void CurveViewWithBannerAndCursor::reloadSelection() {
  float pixelXSelection = roundf(floatToPixel(Axis::Horizontal, m_curveViewWindowWithCursor->xCursorPosition()));
  float pixelYSelection = roundf(floatToPixel(Axis::Vertical, m_curveViewWindowWithCursor->yCursorPosition()));
  KDRect dirtyZone(KDRect(pixelXSelection - k_cursorSize/2, pixelYSelection - k_cursorSize/2, k_cursorSize, k_cursorSize));
  markRectAsDirty(dirtyZone);
  layoutSubviews();
  bannerView()->reload();
}

void CurveViewWithBannerAndCursor::layoutSubviews() {
  KDCoordinate xCursorPixelPosition = roundf(floatToPixel(Axis::Horizontal, m_curveViewWindowWithCursor->xCursorPosition()));
  KDCoordinate yCursorPixelPosition = roundf(floatToPixel(Axis::Vertical, m_curveViewWindowWithCursor->yCursorPosition()));
  KDRect cursorFrame(xCursorPixelPosition - k_cursorSize/2, yCursorPixelPosition - k_cursorSize/2, k_cursorSize, k_cursorSize);
  if (!m_mainViewSelected) {
    cursorFrame = KDRectZero;
  }
  m_cursorView.setFrame(cursorFrame);
  CurveViewWithBanner::layoutSubviews();
}

int CurveViewWithBannerAndCursor::numberOfSubviews() const {
  return 2;
};

View * CurveViewWithBannerAndCursor::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_cursorView;
  }
  return bannerView();
}
