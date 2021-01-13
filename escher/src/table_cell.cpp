#include <escher/table_cell.h>
#include <escher/palette.h>
#include <escher/metric.h>
#include <algorithm>
#include <ion.h>
// #include <iostream>

namespace Escher {

TableCell::TableCell() :
  Bordered(),
  HighlightCell()
{
}

int TableCell::numberOfSubviews() const {
  return (labelView() != nullptr) + (subLabelView()!= nullptr) + (accessoryView()!= nullptr);
}

View * TableCell::subviewAtIndex(int index) {
  if (index == 0) {
    return labelView();
  }
  if (index == 1 && subLabelView() != nullptr) {
    return subLabelView();
  }
  assert(index == 2 || (index == 1 && subLabelView() == nullptr));
  return accessoryView();
}

/*TODO: uniformize where margins are added. Sometimes the subview has included
 * margins (like ExpressionView), sometimes the subview has no margins (like
 * MessageView) which prevents us to handle margins only here. */

KDCoordinate TableCell::minimalHeightForOptimalDisplay(View * label, View * subLabel, View * accessory, KDCoordinate width) {
  KDSize labelSize = label ? label->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize subLabelSize = subLabel ? subLabel->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize accessorySize = accessory ? accessory->minimalSizeForOptimalDisplay() : KDSizeZero;
  // Compute available width for Label and subLabel
  width -= Metric::CellLeftMargin + Metric::CellRightMargin;
  if (accessory) {
    width -= Metric::CellHorizontalElementMargin + accessorySize.width();
  }

  KDCoordinate labelHeight = labelSize.height();
  KDCoordinate subLabelHeight = subLabelSize.height();
  // Compute minimal Height for Label and subLabel
  KDCoordinate labelsHeight;
  if (label && subLabel && labelSize.width() + Metric::CellHorizontalElementMargin + subLabelSize.width() > width) {
    // Two rows are required to fit content
    labelsHeight = labelHeight + Metric::CellVerticalElementMargin + subLabelHeight;
  } else {
    // Label and subLabel fit in the same row
    labelsHeight = std::max<KDCoordinate>(labelHeight, subLabelHeight);
  }
  return k_separatorThickness + Metric::CellTopMargin + std::max<KDCoordinate>(labelsHeight, accessorySize.height()) + Metric::CellBottomMargin;
}

KDSize TableCell::minimalSizeForOptimalDisplay() const {
  // Find a way to get bounds().width(). Can be 280 instead of 266 with params + handle max nb of rows
  KDCoordinate expectedWidth = Ion::Display::Width - Metric::PopUpLeftMargin - Metric::PopUpRightMargin; // m_frame.width(); //
  // if (m_frame.width() + m_frame.height() + bounds().width() + bounds().height() > 0) {
  //   std::cout << m_frame.width() << " , " << m_frame.height() << " - " << bounds().width() << " , " << bounds().height() << "\n";
  // }
  return KDSize(expectedWidth, minimalHeightForOptimalDisplay(labelView(), subLabelView(), accessoryView(), expectedWidth));
}

KDCoordinate cropIfOverflow(KDCoordinate value, KDCoordinate max) {
  // assert(value <= max);
  if (value > max) {
    Ion::Display::pushRectUniform(KDRect(0,0,KDSize(10,10)), KDColorRed);
  }
  return value > max ? max : value;
}

void TableCell::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();

  if (width == 0 || height == 0) {
    return;
  }

  View * label = labelView();
  View * subLabel = subLabelView();
  View * accessory = accessoryView();

  KDSize labelSize = label ? label->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize subLabelSize = subLabel ? subLabel->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize accessorySize = accessory ? accessory->minimalSizeForOptimalDisplay() : KDSizeZero;

  KDCoordinate y = 0;
  KDCoordinate x = 0;
  // Apply margins on every side
  width -= Metric::CellLeftMargin + Metric::CellRightMargin;
  x += Metric::CellLeftMargin;
  height -= k_separatorThickness + Metric::CellTopMargin + Metric::CellBottomMargin;
  y += k_separatorThickness + Metric::CellTopMargin;

  if (width < 0 || height < 0) {
    assert(false);
    return;
  }

  // If cell contains an accessory, place it first and update remaining space.
  if (accessory) {
    KDCoordinate accessoryWidth = cropIfOverflow(accessorySize.width(), width - (label || subLabel ? Metric::CellHorizontalElementMargin : 0));
    KDCoordinate accessoryHeight = cropIfOverflow(accessorySize.height(), height);

    // Accessory must be vertically centered on the entire cell height.
    KDCoordinate verticalCenterOffset = (height - accessorySize.height() + 1) / 2;
    // Set accessory frame
    accessory->setFrame(KDRect(x + width - accessoryWidth, y + verticalCenterOffset, accessoryWidth, accessoryHeight), force);
    // Update remaining space, add margin before accessory
    width -= accessoryWidth + Metric::CellHorizontalElementMargin;
  }

  KDCoordinate labelHeight = cropIfOverflow(labelSize.height(), height);
  KDCoordinate labelWidth = cropIfOverflow(labelSize.width(), width);
  KDCoordinate subLabelHeight = cropIfOverflow(subLabelSize.height(), height);
  KDCoordinate subLabelWidth = cropIfOverflow(subLabelSize.width(), width);
  bool singleRow = true;

  // The shortest element on a same row is vertically centered.
  KDCoordinate maxHeight = std::max<KDCoordinate>(labelHeight, subLabelHeight);

  if (label && subLabel && labelWidth + Metric::CellHorizontalElementMargin + subLabelWidth > width) {
    // Two rows are needed to fit both label and subLabel.
    singleRow = false;
    // No need to center the shortest element
    maxHeight = labelHeight;
    // assert(labelWidth <= width && subLabelWidth <= width);
    if (labelWidth > width) {
      labelWidth = width;
    }
    if (subLabelWidth > width) {
      subLabelWidth = width;
    }
    subLabelHeight = cropIfOverflow(subLabelHeight, height - labelHeight - Metric::CellVerticalElementMargin);
  }

  if (label) {
    KDCoordinate verticalCenterOffset = (maxHeight - labelHeight + 1) / 2;
    label->setFrame(KDRect(x, y + verticalCenterOffset, labelWidth, labelHeight), force);
  }

  if (subLabel) {
    if (singleRow) {
      // SubLabel is aligned to the right
      x += width - subLabelWidth;
      subLabelWidth = cropIfOverflow(subLabelWidth, width - labelWidth - Metric::CellHorizontalElementMargin);
    } else {
      // SubLabel is aligned to the left, no need to center the shortest element
      maxHeight = subLabelHeight;
      if (label) {
        // Add vertical separation margin if there was a label
        y += labelHeight + Metric::CellVerticalElementMargin;
      }
    }
    KDCoordinate verticalCenterOffset = (maxHeight - subLabelHeight + 1) / 2;
    subLabel->setFrame(KDRect(x, y + verticalCenterOffset, subLabelWidth, subLabelHeight), force);
  }
}

void TableCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backColor = isHighlighted() ? Palette::Select : backgroundColor();
  drawInnerRect(ctx, bounds(), backColor);
  drawBorderOfRect(ctx, bounds(), Palette::GrayBright);
}

}
