#include <escher/table_cell.h>
#include <escher/palette.h>
#include <escher/metric.h>
#include <algorithm>

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

void TableCell::layoutSubviews(bool force) {
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
    y = std::max<KDCoordinate>(y, height - k_separatorThickness - withMargin(accessorySize.height(), Metric::TableCellVerticalMargin) - withMargin(subAccessorySize.height(), 0));
    if (subAccessory) {
      KDCoordinate subAccessoryHeight = std::min<KDCoordinate>(subAccessorySize.height(), height - y - k_separatorThickness - Metric::TableCellVerticalMargin);
      accessory->setFrame(KDRect(horizontalMargin, y, width - 2*horizontalMargin, subAccessoryHeight), force);
      y += subAccessoryHeight;
    }
    horizontalMargin = k_separatorThickness + accessoryMargin();
    y = std::max<KDCoordinate>(y, height - k_separatorThickness - withMargin(accessorySize.height(), Metric::TableCellVerticalMargin));
    if (accessory) {
      KDCoordinate accessoryHeight = std::min<KDCoordinate>(accessorySize.height(), height - y - k_separatorThickness - Metric::TableCellVerticalMargin);
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
}

void TableCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;
  drawInnerRect(ctx, bounds(), backgroundColor);
  drawBorderOfRect(ctx, bounds(), Palette::GreyBright);
}
