#include "horizontal_or_vertical_layout.h"

using namespace Probability;

// TODO this can be HEAVILLY factorized

KDSize Probability::VerticalLayout::minimalSizeForOptimalDisplay() const {
  int requiredWidth = 0, requiredHeight = 0;
  KDSize requiredSize(0, 0);
  KDCoordinate proposedWidth = bounds().width() - 2 * m_marginX;
  for (int i = 0; i < numberOfSubviews(); i++) {
    Escher::View * subview = const_cast<VerticalLayout *>(this)->subviewAtIndex(i);
    subview->setSize(KDSize(proposedWidth, subview->bounds().height()));
    requiredSize = subview->minimalSizeForOptimalDisplay();
    requiredHeight += requiredSize.height();
    requiredWidth = fmaxf(requiredWidth, requiredSize.width());
  }
  return KDSize(requiredWidth, requiredHeight);
}

void VerticalLayout::layoutSubviews(bool force) {
  KDRect frame = bounds();
  KDCoordinate availableHeight = frame.height() - 2 * m_marginY;
  KDRect proposedFrame = KDRect(m_marginX, m_marginY, frame.width() - 2 * m_marginX, 0);
  for (int i = 0; i < numberOfSubviews(); i++) {
    subviewAtIndex(i)->setSize(KDSize(proposedFrame.width(), availableHeight));
    int height = subviewAtIndex(i)->minimalSizeForOptimalDisplay().height();
    proposedFrame = KDRect(proposedFrame.x(),
                           proposedFrame.y() + proposedFrame.height(),
                           proposedFrame.width(),
                           height);
    subviewAtIndex(i)->setFrame(proposedFrame, false);
    availableHeight -= height;
    assert(availableHeight >= 0);
  }
}

KDSize Probability::HorizontalLayout::minimalSizeForOptimalDisplay() const {
  int requiredWidth = 0, requiredHeight = 0;
  KDSize requiredSize(0, 0);
  KDRect proposedFrame = KDRect(0, 0, 0, bounds().height());
  for (int i = 0; i < numberOfSubviews(); i++) {
    Escher::View * subview = const_cast<HorizontalLayout *>(this)->subviewAtIndex(i);
    subview->setSize(KDSize(subview->bounds().width(), bounds().height()));
    requiredSize = subview->minimalSizeForOptimalDisplay();
    requiredWidth += requiredSize.width();
    requiredHeight = fmax(requiredHeight, requiredSize.height());
  }
  return KDSize(requiredWidth, requiredHeight);
}

void HorizontalLayout::layoutSubviews(bool force) {
  KDRect frame = bounds();
  KDCoordinate availableWidth = frame.width();
  KDRect proposedFrame = KDRect(0, 0, 0, frame.height());
  for (int i = 0; i < numberOfSubviews(); i++) {
    subviewAtIndex(i)->setSize(KDSize(availableWidth, frame.height()));
    int width = subviewAtIndex(i)->minimalSizeForOptimalDisplay().width();
    proposedFrame = KDRect(proposedFrame.x() + proposedFrame.width(), 0, width, frame.height());
    subviewAtIndex(i)->setFrame(proposedFrame, false);
    availableWidth -= width;
    assert(availableWidth >= 0);
  }
}
