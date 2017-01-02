#ifndef APPS_CURVE_VIEW_WITH_BANNER_AND_CURSOR_H
#define APPS_CURVE_VIEW_WITH_BANNER_AND_CURSOR_H

#include <escher.h>
#include "curve_view_window_with_cursor.h"
#include "curve_view_with_banner.h"
#include "cursor_view.h"

class CurveViewWithBannerAndCursor : public CurveViewWithBanner {
public:
  CurveViewWithBannerAndCursor(CurveViewWindowWithCursor * curveViewWindowWithCursor = nullptr, float topMarginFactor = 0.0f,
    float rightMarginFactor = 0.0f, float bottomMarginFactor = 0.0f, float leftMarginFactor = 0.0f);
  void reload() override;
  void reloadSelection() override;
protected:
  constexpr static KDCoordinate k_cursorSize = 9;
  void layoutSubviews() override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;

  CursorView m_cursorView;
  CurveViewWindowWithCursor * m_curveViewWindowWithCursor;
};

#endif
