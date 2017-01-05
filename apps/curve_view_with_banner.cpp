#include "curve_view_with_banner.h"
#include <assert.h>
#include <math.h>

CurveViewWithBanner::CurveViewWithBanner(CurveViewWindow * curveViewWindow, BannerView * bannerView,
    float topMarginFactor, float rightMarginFactor, float bottomMarginFactor, float leftMarginFactor) :
  CurveView(curveViewWindow, topMarginFactor, rightMarginFactor, bottomMarginFactor, leftMarginFactor),
  m_mainViewSelected(true),
  m_bannerView(bannerView)
{
}

void CurveViewWithBanner::reload() {
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
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
  return m_bannerView;
}

void CurveViewWithBanner::layoutSubviews() {
  m_bannerView->setFrame(bounds());
  KDCoordinate bannerHeight = m_bannerView->minimalSizeForOptimalDisplay().height();
  KDRect bannerFrame(KDRect(0, bounds().height()- bannerHeight, bounds().width(), bannerHeight));
  if (!m_mainViewSelected) {
    bannerFrame = KDRectZero;
  }
  m_bannerView->setFrame(bannerFrame);
}
