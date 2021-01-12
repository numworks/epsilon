#include <escher/table_cell.h>
#include <escher/palette.h>
#include <escher/metric.h>
#include <algorithm>
#include <ion.h>

namespace Escher {

TableCell::TableCell(Layout layout) :
  Bordered(),
  HighlightCell(),
  m_layout(layout)
{
}

View * TableCell::labelView() const {
  return nullptr;
}

View * TableCell::accessoryView() const {
  return nullptr;
}

View * TableCell::subAccessoryView() const {
  return nullptr;
}

int TableCell::numberOfSubviews() const {
  return (labelView() != nullptr) + (accessoryView()!= nullptr) + (subAccessoryView()!= nullptr);
}

View * TableCell::subviewAtIndex(int index) {
  if (index == 0) {
    return labelView();
  }
  if (index == 1) {
    return accessoryView();
  }
  return subAccessoryView();
}

/*TODO: uniformize where margins are added. Sometimes the subview has included
 * margins (like ExpressionView), sometimes the subview has no margins (like
 * MessageView) which prevents us to handle margins only here. */

KDCoordinate withMargin(KDCoordinate length, KDCoordinate margin) {
  return length == 0 ? 0 : length + margin;
}

KDCoordinate TableCell::minimalHeightForOptimalDisplay(View * label, View * subLabel, View * accessory, KDCoordinate width) {
  if (accessory && !subLabel) {
    subLabel = accessory;
    accessory = nullptr;
  }

  KDSize labelSize = label ? label->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize subLabelSize = subLabel ? subLabel->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize accessorySize = accessory ? accessory->minimalSizeForOptimalDisplay() : KDSizeZero;

  KDCoordinate labelHeight = labelSize.height();
  KDCoordinate labelWidth = labelSize.width();
  KDCoordinate subLabelHeight = subLabelSize.height();
  KDCoordinate subLabelWidth = subLabelSize.width();
  KDCoordinate accessoryHeight = accessorySize.height();
  KDCoordinate accessoryWidth = accessorySize.width();

  width -= Metric::CellLeftMargin + Metric::CellRightMargin;
  if (accessory) {
    width -= Metric::CellHorizontalElementMargin + accessoryWidth;
  }

  bool singleRow = true;
  if (label && subLabel && labelWidth + Metric::CellHorizontalElementMargin + subLabelWidth > width) {
    singleRow = false;
  }
  KDCoordinate labelsHeight;

  if (singleRow) {
    labelsHeight = std::max<KDCoordinate>(labelHeight, subLabelHeight);
  } else {
    labelsHeight = labelHeight + Metric::CellVerticalElementMargin + subLabelHeight;
  }

  return k_separatorThickness + Metric::CellTopMargin + std::max<KDCoordinate>(labelsHeight, accessoryHeight) + Metric::CellBottomMargin;
}

KDSize TableCell::minimalSizeForOptimalDisplay() const {
  // Find a way to get bounds().width(). Can be 280 instead of 266 with params + handle max nb of rows
  KDCoordinate expectedWidth = Ion::Display::Width - Metric::PopUpLeftMargin - Metric::PopUpRightMargin;
  return KDSize(expectedWidth, minimalHeightForOptimalDisplay(labelView(), subAccessoryView(), accessoryView(), expectedWidth));
}

void TableCell::layoutSubviews(bool force) {
  // TODO : rename label, accessory and subAccessory
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();

  if (width == 0 && height == 0) {
    return;
  }

  View * label = labelView();
  View * subLabel = subAccessoryView();
  View * accessory = accessoryView();

  if (accessory && !subLabel) {
    subLabel = accessory;
    accessory = nullptr;
  }

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

  assert(width > 0 && height > 0);

  // If cell contains an accessory, place it first and update remaining space.
  if (accessory) {
    // TODO replace these with assert
    KDCoordinate accessoryWidth = std::min<KDCoordinate>(accessorySize.width(), width);
    // Accessory must be vertically centered on the entire cell height.
    accessory->setFrame(
      KDRect(
        x + width - accessoryWidth,
        y,
        accessoryWidth,
        height) //accessoryHeight center might be automatic
      , force);
    // Update remaining space
    width -= accessoryWidth + Metric::CellHorizontalElementMargin;
  }
  // TODO First element is the principal content of the cell, it has a big font and black color.
  // TODO Second element is small font and gray
  // TODO replace these with assert
  KDCoordinate labelHeight = std::min<KDCoordinate>(labelSize.height(), height);
  KDCoordinate labelWidth = std::min<KDCoordinate>(labelSize.width(), width);
  KDCoordinate subLabelHeight = std::min<KDCoordinate>(subLabelSize.height(), height);
  KDCoordinate subLabelWidth = std::min<KDCoordinate>(subLabelSize.width(), width);

  bool singleRow = true;
  // If there are two elements on the same row, the shortest is vertically centered.
  KDCoordinate maxHeight = std::max<KDCoordinate>(labelHeight, subLabelHeight);
  if (label && subLabel && labelWidth + Metric::CellHorizontalElementMargin + subLabelWidth > width) {
    // Two rows are needed to fit both label and subLabel.
    singleRow = false;
    maxHeight = labelHeight;
  }

  if (label) {
    // KDCoordinate verticalCenterOffset = (maxHeight - labelHeight) / 2; // might be automatic
    label->setFrame(
      KDRect(
        x,
        y, //  + verticalCenterOffset
        labelWidth,
        maxHeight)
      , force);
  }

  if (subLabel) {
      // Update remaining space
    if (singleRow) {
      // SubLabel is aligned to the right
      x += width - subLabelWidth;
    } else {
      // SubLabel is aligned to the left
      maxHeight = subLabelHeight;
      // Add vertical separation margin if there was a label
      y += labelHeight + (label ? Metric::CellVerticalElementMargin : 0);
    }
    // KDCoordinate verticalCenterOffset = (maxHeight - labelHeight) / 2; // might be automatic
    subLabel->setFrame(
      KDRect(
        x,
        y, //  + verticalCenterOffset
        subLabelWidth,
        maxHeight)// row height
      , force);
  }
#if 0
  /* TODO: this code is awful. However, this should handle multiples cases
   * (subviews are not defined, margins are overriden...) */
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  View * label = labelView();
  View * accessory = accessoryView();
  View * subAccessory = subAccessoryView();
  KDSize labelSize = label ? label->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize accessorySize = accessory ? accessory->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize subAccessorySize = subAccessory ? subAccessory->minimalSizeForOptimalDisplay() : KDSizeZero;
  if (m_layout == Layout::Vertical) {
    /*
     * Vertically:
     * ----------------
     * ----------------
     * Line separator
     * ----------------
     * k_verticalMargin
     * ----------------
     *     LABEL
     * ----------------
     * k_verticalMargin
     * ----------------
     *       .
     *       . [White space if possible, otherwise LABEL overlaps SUBACCESSORY and so on]
     *       .
     * ----------------
     *  SUBACCESSORY
     * ----------------
     *   ACCESSORY
     * ----------------
     * k_verticalMargin
     * ----------------
     * Line separator
     * ----------------
     * ----------------
     *
     *
     *  Horizontally:
     * || Line separator | margin* | SUBVIEW | margin* | Line separator ||
     *
     * * = margin can either be labelMargin(), accessoryMargin() or k_horizontalMargin depending on the subview
     *
     * */
    KDCoordinate horizontalMargin = k_separatorThickness + labelMargin();
    KDCoordinate y = k_separatorThickness;
    if (label) {
      y += k_verticalMargin;
      KDCoordinate labelHeight = std::min<KDCoordinate>(labelSize.height(), height - y - k_separatorThickness - k_verticalMargin);
      label->setFrame(KDRect(horizontalMargin, y, width-2*horizontalMargin, labelHeight), force);
      y += labelHeight + k_verticalMargin;
    }
    horizontalMargin = k_separatorThickness + k_horizontalMargin;
    y = std::max<KDCoordinate>(y, height - k_separatorThickness - withMargin(accessorySize.height(), k_verticalMargin) - withMargin(subAccessorySize.height(), 0));
    if (subAccessory) {
      KDCoordinate subAccessoryHeight = std::min<KDCoordinate>(subAccessorySize.height(), height - y - k_separatorThickness - k_verticalMargin);
      assert(accessory);
      accessory->setFrame(KDRect(horizontalMargin, y, width - 2*horizontalMargin, subAccessoryHeight), force);
      y += subAccessoryHeight;
    }
    horizontalMargin = k_separatorThickness + accessoryMargin();
    y = std::max<KDCoordinate>(y, height - k_separatorThickness - withMargin(accessorySize.height(), k_verticalMargin));
    if (accessory) {
      KDCoordinate accessoryHeight = std::min<KDCoordinate>(accessorySize.height(), height - y - k_separatorThickness - k_verticalMargin);
      accessory->setFrame(KDRect(horizontalMargin, y, width - 2*horizontalMargin, accessoryHeight), force);
    }
  } else {
    /*
     * Vertically:
     * ----------------
     * ----------------
     * Line separator
     * ----------------
     *    SUBVIEW
     * ----------------
     * Line separator
     * ----------------
     * ----------------
     *
     *  Horizontally:
     * || Line separator | Label margin | LABEL | Label margin | ...
     *      [ White space if possible otherwise the overlap can be from left to
     *      right subviews or the contrary ]
     *
     *  ... | SUBACCESSORY | ACCESSORY | Accessory margin | Line separator ||
     *
     * */

    KDCoordinate verticalMargin = k_separatorThickness;
    KDCoordinate x = 0;
    KDCoordinate labelX = k_separatorThickness + labelMargin();
    KDCoordinate subAccessoryX = std::max(k_separatorThickness + k_horizontalMargin, width - k_separatorThickness - withMargin(accessorySize.width(), accessoryMargin()) - withMargin(subAccessorySize.width(), 0));
    KDCoordinate accessoryX = std::max(k_separatorThickness + accessoryMargin(), width - k_separatorThickness - withMargin(accessorySize.width(), accessoryMargin()));
    if (label) {
      x = labelX;
      KDCoordinate labelWidth = std::min<KDCoordinate>(labelSize.width(), width - x - k_separatorThickness - labelMargin());
      if (m_layout == Layout::HorizontalRightOverlap) {
        labelWidth = std::min<KDCoordinate>(labelWidth, subAccessoryX - x - labelMargin());
      }
      label->setFrame(KDRect(x, verticalMargin, labelWidth, height-2*verticalMargin), force);
      x += labelWidth + labelMargin();
    }
    if (subAccessory) {
      x = std::max(x, subAccessoryX);
      KDCoordinate subAccessoryWidth = std::min<KDCoordinate>(subAccessorySize.width(), width - x - k_separatorThickness - k_horizontalMargin);
      if (m_layout == Layout::HorizontalRightOverlap) {
        subAccessoryWidth = std::min<KDCoordinate>(subAccessoryWidth, accessoryX - x);
      }
      subAccessory->setFrame(KDRect(x, verticalMargin, subAccessoryWidth, height-2*verticalMargin), force);
      x += subAccessoryWidth;
    }
    if (accessory) {
      x = std::max(x, accessoryX);
      KDCoordinate accessoryWidth = std::min<KDCoordinate>(accessorySize.width(), width - x - k_separatorThickness - accessoryMargin());
      accessory->setFrame(KDRect(x, verticalMargin, accessoryWidth, height-2*verticalMargin), force);
    }
  }
#endif
}

void TableCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backColor = isHighlighted() ? Palette::Select : backgroundColor();
  drawInnerRect(ctx, bounds(), backColor);
  drawBorderOfRect(ctx, bounds(), Palette::GrayBright);
}

}
