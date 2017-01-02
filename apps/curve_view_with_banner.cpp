#include "curve_view_with_banner.h"
#include <assert.h>
#include <math.h>

CurveViewWithBanner::CurveViewWithBanner(CurveViewWindow * curveViewWindow, float topMarginFactor,
    float rightMarginFactor, float bottomMarginFactor, float leftMarginFactor) :
  CurveView(curveViewWindow, topMarginFactor, rightMarginFactor, bottomMarginFactor, leftMarginFactor),
  m_mainViewSelected(true)
{
}

void CurveViewWithBanner::reload() {
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
  bannerView()->reload();
}

bool CurveViewWithBanner::isMainViewSelected() {
  return m_mainViewSelected;
}

void CurveViewWithBanner::selectMainView(bool mainViewSelected) {
  if (m_mainViewSelected != mainViewSelected) {
    m_mainViewSelected = mainViewSelected;
    reloadSelection();
    layoutSubviews();
  }
}

int CurveViewWithBanner::numberOfSubviews() const {
  return 1;
};

View * CurveViewWithBanner::subviewAtIndex(int index) {
  assert(index == 0);
  return bannerView();
}

void CurveViewWithBanner::layoutSubviews() {
  KDRect bannerFrame(KDRect(0, bounds().height()- k_bannerHeight, bounds().width(), k_bannerHeight));
  if (!m_mainViewSelected) {
    bannerFrame = KDRectZero;
  }
  bannerView()->setFrame(bannerFrame);
}
