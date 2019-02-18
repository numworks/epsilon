#include "equation_list_view.h"

namespace Solver {

/* EquationListView */

EquationListView::EquationListView(Responder * parentResponder, TableViewDataSource * dataSource, SelectableTableViewDataSource * selectionDataSource) :
  Responder(parentResponder),
  View(),
  m_braceStyle(BraceStyle::None),
  m_listView(this, dataSource, selectionDataSource),
  m_braceView(),
  m_scrollBraceView(&m_braceView, this)
{
  m_listView.setMargins(0);
  m_listView.setVerticalCellOverlap(0);
  m_listView.setDecoratorType(ScrollView::Decorator::Type::None);
  selectionDataSource->setScrollViewDelegate(this);
  m_scrollBraceView.setMargins(k_margin, k_margin, k_margin, k_margin);
  m_scrollBraceView.setDecoratorType(ScrollView::Decorator::Type::None);
  m_scrollBraceView.setBackgroundColor(KDColorWhite);
}

void EquationListView::setBraceStyle(BraceStyle style) {
  if (m_braceStyle != style) {
    m_braceStyle = style;
    layoutSubviews();
  }
}

void EquationListView::scrollViewDidChangeOffset(ScrollViewDataSource * scrollViewDataSource) {
  m_scrollBraceView.setContentOffset(KDPoint(0, scrollViewDataSource->offset().y()));
  layoutSubviews();
}

int EquationListView::numberOfSubviews() const {
  return 2;
}

View * EquationListView::subviewAtIndex(int index) {
  View * subviews[] = {&m_listView, &m_scrollBraceView};
  return subviews[index];
}

void EquationListView::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_listView);
}

void EquationListView::layoutSubviews() {
  m_listView.setFrame(KDRect(0, 0, bounds().width(), bounds().height()));
  if (m_braceStyle != BraceStyle::None) {
    KDCoordinate braceWidth = m_braceView.minimalSizeForOptimalDisplay().width();
    KDCoordinate braceHeight = m_listView.minimalSizeForOptimalDisplay().height()-2*k_margin;
    braceHeight = m_braceStyle == BraceStyle::OneRowShort ? braceHeight - Metric::StoreRowHeight : braceHeight;
    m_braceView.setSize(KDSize(braceWidth, braceHeight));
    KDCoordinate scrollBraceHeight = m_listView.minimalSizeForOptimalDisplay().height()-offset().y();
    scrollBraceHeight = m_braceStyle == BraceStyle::OneRowShort ? scrollBraceHeight - Metric::StoreRowHeight : scrollBraceHeight;
    m_scrollBraceView.setFrame(KDRect(0, 0, k_braceTotalWidth, scrollBraceHeight));
  } else {
    m_scrollBraceView.setFrame(KDRectZero);
  }
}

/* EquationListView::BraceWidth */

constexpr KDCoordinate braceExtremumHeight = 6;
constexpr KDCoordinate braceExtremumWidth = 10;
constexpr KDCoordinate braceCenterHeight = 15;
constexpr KDCoordinate braceCenterWidth = 4;

const uint8_t topBrace[braceExtremumHeight][braceExtremumWidth] = {
  {0xFF, 0xFF, 0xF7, 0x25, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0xFF, 0x7B, 0x02, 0x90, 0xC4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0x92, 0x18, 0xD7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0x3D, 0xAC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
};

const uint8_t middleBrace[braceCenterHeight][braceCenterWidth] = {
  {0xFF, 0xFF, 0xFF, 0x00},
  {0xFF, 0xFF, 0xD7, 0x29},
  {0xFF, 0xFF, 0xB9, 0x33},
  {0xFF, 0xFF, 0x7B, 0x76},
  {0xFF, 0xFF, 0x2C, 0xC3},
  {0xFF, 0xAA, 0x44, 0xFF},
  {0xFF, 0x38, 0xAB, 0xFF},
  {0x00, 0x00, 0xFF, 0xFF},
  {0xFF, 0x38, 0xAB, 0xFF},
  {0xFF, 0xAA, 0x44, 0xFF},
  {0xFF, 0xFF, 0x2C, 0xC3},
  {0xFF, 0xFF, 0x7B, 0x76},
  {0xFF, 0xFF, 0xB9, 0x33},
  {0xFF, 0xFF, 0xD7, 0x29},
  {0xFF, 0xFF, 0xFF, 0x00},
};

const uint8_t bottomBrace[braceExtremumHeight][braceExtremumWidth] = {
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0x3D, 0xAC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0x92, 0x18, 0xD7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x7B, 0x02, 0x90, 0xC4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xF7, 0x25, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
};

KDColor s_braceWorkingBuffer[60];

void EquationListView::BraceView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
  KDCoordinate height = bounds().height();
  KDCoordinate margin = 3;
  ctx->blendRectWithMask(KDRect(margin, 0, braceExtremumWidth, braceExtremumHeight), KDColorBlack, (const uint8_t *)topBrace, (KDColor *)(s_braceWorkingBuffer));
  ctx->blendRectWithMask(KDRect(0, height/2-braceCenterHeight/2, braceCenterWidth, braceCenterHeight), KDColorBlack, (const uint8_t *)middleBrace, (KDColor *)(s_braceWorkingBuffer));
  ctx->blendRectWithMask(KDRect(margin, height-braceExtremumHeight, braceExtremumWidth, braceExtremumHeight), KDColorBlack, (const uint8_t *)bottomBrace, (KDColor *)(s_braceWorkingBuffer));
  ctx->fillRect(KDRect(margin, braceExtremumHeight, 1, height/2-braceCenterHeight/2-braceExtremumHeight), KDColorBlack);
  ctx->fillRect(KDRect(margin, height/2+braceCenterHeight/2, 1, height/2-braceExtremumHeight/2-braceExtremumHeight), KDColorBlack);
}

KDSize EquationListView::BraceView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_braceWidth, bounds().height());
}

}

