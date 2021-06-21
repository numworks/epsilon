#include "horizontal_or_vertical_layout.h"

using namespace Probability;

KDSize Probability::VerticalLayout::minimalSizeForOptimalDisplay() const {
  int requiredWidth = 0, requiredHeight = 0;
  KDSize requiredSize(0, 0);
  KDRect proposedFrame = KDRect(0, 0, bounds().width(), 0);
  for (int i = 0; i < numberOfSubviews(); i++) {
    Escher::View * subview = const_cast<VerticalLayout *>(this)->subviewAtIndex(i);
    subview->setFrame(KDRect(0, proposedFrame.y() + proposedFrame.height(), bounds().width(), 0),
                      false);
    requiredSize = subview->minimalSizeForOptimalDisplay();
    requiredHeight += requiredSize.height();
    requiredWidth = fmaxf(requiredWidth, requiredSize.width());
  }
  return KDSize(requiredWidth, requiredHeight);
}

void VerticalLayout::layoutSubviews(bool force) {
  KDRect frame = bounds();
  KDCoordinate availableHeight = frame.height();
  KDRect proposedFrame = KDRect(0, 0, frame.width(), 0);
  for (int i = 0; i < numberOfSubviews(); i++) {
    subviewAtIndex(i)->setFrame(
        KDRect(0, proposedFrame.y() + proposedFrame.height(), frame.width(), availableHeight),
        false);
    int height = subviewAtIndex(i)->minimalSizeForOptimalDisplay().height();
    proposedFrame = KDRect(0, proposedFrame.y() + proposedFrame.height(), frame.width(), height);
    subviewAtIndex(i)->setFrame(proposedFrame, false);
    availableHeight -= height;
    assert(availableHeight >= 0);
  }
}

KDSize Probability::HorizontalLayout::minimalSizeForOptimalDisplay() const {
  int requiredWidth = 0, requiredHeight = 0;
  KDSize requiredSize(0, 0);
  for (int i = 0; i < numberOfSubviews(); i++) {
    Escher::View * subview = const_cast<HorizontalLayout *>(this)->subviewAtIndex(i);
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
    subviewAtIndex(i)->setFrame(
        KDRect(proposedFrame.x() + proposedFrame.width(), 0, availableWidth, frame.height()),
        false);
    int width = subviewAtIndex(i)->minimalSizeForOptimalDisplay().width();
    proposedFrame = KDRect(proposedFrame.x() + proposedFrame.width(), 0, width, frame.height());
    subviewAtIndex(i)->setFrame(proposedFrame, false);
    availableWidth -= width;
    assert(availableWidth >= 0);
  }
}
