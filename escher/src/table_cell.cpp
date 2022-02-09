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

KDCoordinate TableCell::minimalHeightForOptimalDisplay(const View * label, const View * subLabel, const View * accessory, KDCoordinate minAccessoryWidth, KDCoordinate width) {
  KDSize labelSize = label ? label->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize subLabelSize = subLabel ? subLabel->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize accessorySize = accessory ? accessory->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDCoordinate accessoryWidth =
      std::max(accessorySize.width(), minAccessoryWidth);

  bool singleRow = singleRowMode(width, label, subLabel, accessoryWidth);

  KDCoordinate labelHeight = labelSize.height();
  KDCoordinate subLabelHeight = subLabelSize.height();
  // Compute minimal Height for Label and subLabel
  KDCoordinate labelsHeight =
      singleRow ? std::max(labelHeight, subLabelHeight)
                : labelHeight + Metric::CellVerticalElementMargin + subLabelHeight;
  /* Space required for bottom separator is not accounted for as it overlaps
   * with neighbor cells. It is added to the frame in TableView, and exploited
   * when layouting subviews. */
  return k_separatorThickness + Metric::CellTopMargin + std::max(labelsHeight, accessorySize.height()) + Metric::CellBottomMargin;
}

KDSize TableCell::minimalSizeForOptimalDisplay() const {
  // TableCell's available width is necessary to compute its minimal height.
  KDCoordinate expectedWidth = m_frame.width();
  assert(expectedWidth > 0);
  return KDSize(expectedWidth,
                minimalHeightForOptimalDisplay(labelView(),
                                               subLabelView(),
                                               accessoryView(),
                                               accessoryMinimalWidthOverridden(),
                                               expectedWidth));
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
  KDCoordinate xEnd = width + leftOffset;
  constexpr KDCoordinate topOffset = k_separatorThickness + Metric::CellTopMargin;
  y += topOffset;
  height -= topOffset + Metric::CellBottomMargin;

  assert(width > 0 && height > 0);
  KDCoordinate labelHeight = cropIfOverflow(labelSize.height(), height);
  KDCoordinate labelWidth = cropIfOverflow(labelSize.width(), width);
  KDCoordinate subLabelHeight = cropIfOverflow(subLabelSize.height(), height);
  KDCoordinate subLabelWidth = shouldHideSublabel() ? 0
                                                    : cropIfOverflow(subLabelSize.width(), width);
  KDCoordinate accessoryHeight = cropIfOverflow(accessorySize.height(), height);
  KDCoordinate accessoryWidth = cropIfOverflow(
      std::max(accessorySize.width(), accessoryMinimalWidthOverridden()),
      width);

  bool singleRow = singleRowMode(bounds().width(), label, subLabel, accessoryWidth);

  KDRect labelRect = KDRectZero, subLabelRect = KDRectZero, accessoryRect = KDRectZero;

  if (singleRow) {  // Single row -> align vertically each view
    // Label on the left, aligned vertically
    labelRect = KDRect(x, y + (height - labelHeight) / 2, labelWidth, labelHeight);
    x += labelWidth + Metric::CellHorizontalElementMargin;

    if (shouldAlignSublabelRight() & !giveAccessoryAllWidth()) {
      // Align SubLabel right
      x = xEnd - subLabelWidth;
      if (accessoryWidth > 0) {
        // Account for both accessoryWidth and the additional horizontal margin.
        x -= (accessoryWidth + Metric::CellHorizontalElementMargin);
      }
    }

    if (!shouldHideSublabel()) {
      subLabelRect = KDRect(x, y + (height - subLabelHeight) / 2, subLabelWidth, subLabelHeight);
      x += subLabelWidth + Metric::CellHorizontalElementMargin;
    }

    KDCoordinate accessoryY = y + (height - accessoryHeight) / 2;
    if (giveAccessoryAllWidth()) {
      accessoryRect = KDRect(x, accessoryY, xEnd - x, accessoryHeight);
    } else {
      accessoryRect = KDRect(xEnd - accessoryWidth, accessoryY, accessoryWidth, accessoryHeight);
    }
  } else {  // Two rows
    labelRect = KDRect(x, y, labelWidth, labelHeight);
    /* Even if sublabel should be aligned to the right, we want it aligned left
     * in a two row configuration. */
    subLabelRect = KDRect(x, y + labelHeight + Metric::CellVerticalElementMargin, subLabelWidth, subLabelHeight);
    KDCoordinate accessoryY = y + (shouldAlignLabelAndAccessory() ? 0 : (height - accessoryHeight) / 2);
    if (giveAccessoryAllWidth()) {
      KDCoordinate maxX =
          leftOffset + Metric::CellVerticalElementMargin +
          (shouldAlignLabelAndAccessory() ? labelWidth : std::max(labelWidth, subLabelWidth));
      accessoryRect = KDRect(maxX, accessoryY, xEnd - maxX, accessoryHeight);
    } else {
      accessoryRect = KDRect(xEnd - accessoryWidth, accessoryY, accessoryWidth, accessoryHeight);
    }
  }

  // Set frames
  setFrameIfViewExists(label, labelRect, force);
  setFrameIfViewExists(subLabel, subLabelRect, force);
  setFrameIfViewExists(accessory, accessoryRect, force);
  // Assert no subview intersects
  assert(subviewsCanOverlap() || (!labelRect.intersects(subLabelRect) && !subLabelRect.intersects(accessoryRect) && !accessoryRect.intersects(labelRect)));
}

bool TableCell::singleRowMode() const {
  return singleRowMode(
      m_frame.width(),
      labelView(),
      subLabelView(),
      std::max(accessoryView() ? accessoryView()->minimalSizeForOptimalDisplay().width()
                               : KDCoordinate(0),
               accessoryMinimalWidthOverridden()));
}

bool TableCell::singleRowMode(KDCoordinate width,
                              const View * labelView,
                              const View * sublabelView,
                              KDCoordinate accessoryWidth) {
  KDSize labelSize = labelView ? labelView->minimalSizeForOptimalDisplay() : KDSizeZero;
  KDSize subLabelSize = sublabelView ? sublabelView->minimalSizeForOptimalDisplay() : KDSizeZero;

  constexpr KDCoordinate leftOffset = k_separatorThickness + Metric::CellLeftMargin;
  width -= leftOffset + Metric::CellRightMargin + k_separatorThickness;

  KDCoordinate minimalWidth = labelSize.width() +
                              Metric::CellHorizontalElementMargin +
                              subLabelSize.width();
  if (accessoryWidth > 0) {
    minimalWidth += Metric::CellHorizontalElementMargin + accessoryWidth;
  }

  bool singleRow = (minimalWidth < width) || labelView == nullptr ||
                   sublabelView == nullptr || labelSize.width() == 0 ||
                   subLabelSize.width() == 0;
  return singleRow;
}

void TableCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backColor = isHighlighted() ? Palette::Select : backgroundColor();
  drawInnerRect(ctx, bounds(), backColor);
  drawBorderOfRect(ctx, bounds(), Palette::GrayBright);
}

void TableCell::setFrameIfViewExists(View * v, KDRect rect, bool force) {
  if (v) {
    v->setFrame(rect, force);
  }
}

}
