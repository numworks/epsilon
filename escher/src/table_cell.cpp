#include <escher/table_cell.h>
#include <escher/palette.h>
#include <escher/metric.h>
#include <algorithm>
#include <ion/display.h>

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
    return const_cast<View *>(labelView());
  }
  if (index == 1 && subLabelView() != nullptr) {
    return  const_cast<View *>(subLabelView());
  }
  assert(index == 2 || (index == 1 && subLabelView() == nullptr));
  return  const_cast<View *>(accessoryView());
}

KDCoordinate TableCell::minimalHeightForOptimalDisplay(const View * label, const View * subLabel, const View * accessory, KDCoordinate width, bool giveAccessoryAllWidth) {
  KDSize labelSize = label ? label->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize subLabelSize = subLabel ? subLabel->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize accessorySize = accessory ? accessory->minimalSizeForOptimalDisplay() : KDSizeZero;
  // Compute available width for Label and subLabel
  width -= Metric::CellLeftMargin + Metric::CellRightMargin + 2*k_separatorThickness ;
  if (accessory) {
    width -= Metric::CellHorizontalElementMargin + accessorySize.width();
  }

  KDCoordinate labelHeight = labelSize.height();
  KDCoordinate subLabelHeight = subLabelSize.height();
  // Compute minimal Height for Label and subLabel
  KDCoordinate labelsHeight;
  if (label && subLabel && (labelSize.width() + Metric::CellHorizontalElementMargin + subLabelSize.width() > width || giveAccessoryAllWidth)) {
    // Two rows are required to fit content
    labelsHeight = labelHeight + Metric::CellVerticalElementMargin + subLabelHeight;
  } else {
    // Label and subLabel fit in the same row
    labelsHeight = std::max<KDCoordinate>(labelHeight, subLabelHeight);
  }
  /* Space required for bottom separator is not accounted for as it overlaps
   * with neighbor cells. It is added to the frame in TableView, and exploited
   * when layouting subviews. */
  return k_separatorThickness + Metric::CellTopMargin + std::max<KDCoordinate>(labelsHeight, accessorySize.height()) + Metric::CellBottomMargin;
}

KDSize TableCell::minimalSizeForOptimalDisplay() const {
  // TableCell's available width is necessary to compute its minimal height.
  KDCoordinate expectedWidth = m_frame.width();
  assert(expectedWidth > 0);
  return KDSize(expectedWidth, minimalHeightForOptimalDisplay(labelView(), subLabelView(), accessoryView(), expectedWidth, giveAccessoryAllWidth()));
}

KDCoordinate cropIfOverflow(KDCoordinate value, KDCoordinate max) {
  return std::min<KDCoordinate>(max, value);
}

void TableCell::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();

  if (width == 0 || height == 0) {
    return;
  }

  View * label = const_cast<View *>(labelView());
  View * subLabel = const_cast<View *>(subLabelView());
  View * accessory = const_cast<View *>(accessoryView());

  KDSize labelSize = label ? label->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize subLabelSize = subLabel ? subLabel->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize accessorySize = accessory ? accessory->minimalSizeForOptimalDisplay() : KDSizeZero;

  KDCoordinate y = 0;
  KDCoordinate x = 0;
  /* Apply margins and separators on every side. At this point, we assume cell's
   * frame has been updated to add bottom and right overlapping borders. */
  // TODO : improve overlapping borders so that we don't need to assume that.
  constexpr KDCoordinate leftOffset = k_separatorThickness + Metric::CellLeftMargin;
  x += leftOffset;
  width -= leftOffset + Metric::CellRightMargin + k_separatorThickness;
  constexpr KDCoordinate topOffset = k_separatorThickness + Metric::CellTopMargin;
  y += topOffset;
  height -= topOffset + Metric::CellBottomMargin + k_separatorThickness;

  assert(width > 0 && height > 0);
  KDCoordinate labelHeight = cropIfOverflow(labelSize.height(), height);
  KDCoordinate labelWidth = cropIfOverflow(labelSize.width(), width);
  KDCoordinate subLabelHeight = cropIfOverflow(subLabelSize.height(), height);
  KDCoordinate subLabelWidth = cropIfOverflow(subLabelSize.width(), width);

  // If cell contains an accessory, place it first and update remaining space.
  if (accessory) {
    // Compute Label and SubLabel minimal required width
    KDCoordinate labelsMinimalWidth = (label || subLabel) ? cropIfOverflow(std::max<KDCoordinate>(labelWidth, subLabelWidth) + Metric::CellHorizontalElementMargin, width) : 0;
    KDCoordinate accessoryWidth;
    if (giveAccessoryAllWidth()) {
      // Accessory takes as much width as available
      accessoryWidth = width - labelsMinimalWidth;
    } else {
      accessoryWidth = cropIfOverflow(accessorySize.width(), width - labelsMinimalWidth);
    }
    KDCoordinate accessoryHeight = cropIfOverflow(accessorySize.height(), height);
    // Depending on alignment, accessory is placed left or right.
    KDCoordinate accessoryX = x;
    if (isAccessoryAlignedRight()) {
      accessoryX += width - accessoryWidth;
    }
    // Accessory must be vertically centered on the entire cell height.
    KDCoordinate availableSpace = height - accessorySize.height();
    // Set accessory frame
    accessory->setFrame(KDRect(accessoryX, y + availableSpace/2, accessoryWidth, accessoryHeight + availableSpace%2), force);
    // Update remaining space, add margin before accessory
    KDCoordinate horizontalOffset = accessoryWidth + Metric::CellHorizontalElementMargin;
    if (!isAccessoryAlignedRight()) {
      x += horizontalOffset;
    }
    width -= horizontalOffset;
  }

  bool singleRow = !(label && subLabel && labelWidth + Metric::CellHorizontalElementMargin + subLabelWidth > width);
  KDCoordinate maxHeight;
  if (singleRow) {
    // On a single row, the shortest element must be vertically centered.
    maxHeight = std::max<KDCoordinate>(labelHeight, subLabelHeight);
  } else {
    // Two rows are needed to fit both label and subLabel.
    maxHeight = labelHeight;
    subLabelHeight = cropIfOverflow(subLabelHeight, height - labelHeight - Metric::CellVerticalElementMargin);
  }

  if (label) {
    /* Label is vertically centered. If available space is odd, the extra pixel
     * is given as extra height. */
    KDCoordinate availableSpace = maxHeight - labelHeight;
    label->setFrame(KDRect(x, y + availableSpace/2, labelWidth, labelHeight + availableSpace%2), force);
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
    // SubLabel is vertically centered, see label's vertical centering comment.
    KDCoordinate availableSpace = maxHeight - subLabelHeight;
    subLabel->setFrame(KDRect(x, y + availableSpace/2, subLabelWidth, subLabelHeight + availableSpace%2), force);
  }
}

void TableCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backColor = isHighlighted() ? Palette::Select : backgroundColor();
  drawInnerRect(ctx, bounds(), backColor);
  drawBorderOfRect(ctx, bounds(), Palette::GrayBright);
}

}
