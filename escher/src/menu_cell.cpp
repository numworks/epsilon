#include <escher/menu_cell.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/metric.h>
#include <escher/palette.h>
#include <ion/display.h>
namespace Escher {

AbstractMenuCell::AbstractMenuCell() : Bordered(), HighlightCell() {}

void AbstractMenuCell::drawRect(KDContext *ctx, KDRect rect) const {
  KDColor backColor = BackgroundColor(isHighlighted());
  drawInnerRect(ctx, bounds(), backColor);
  drawBorderOfRect(ctx, bounds(), Palette::GrayBright);
}

KDSize AbstractMenuCell::minimalSizeForOptimalDisplay() const {
  assert(bounds().width() > 0);
  return KDSize(bounds().width(), minimalHeightForOptimalDisplay());
}

KDCoordinate AbstractMenuCell::minimalHeightForOptimalDisplay() const {
  KDSize thisLabelSize = labelSize();
  KDSize thisSubLabelSize = subLabelSize();
  KDSize thisAccessorySize = accessorySize();

  KDCoordinate contentHeight;
  if (singleRowMode()) {
    contentHeight = std::max(
        thisLabelSize.height(),
        std::max(thisSubLabelSize.height(), thisAccessorySize.height()));
  } else if (shouldAlignLabelAndAccessory()) {
    contentHeight =
        thisSubLabelSize.height() + k_innerVerticalMargin +
        std::max(thisLabelSize.height(), thisAccessorySize.height());
  } else {
    contentHeight = std::max<KDCoordinate>(thisLabelSize.height() +
                                               thisSubLabelSize.height() +
                                               k_innerVerticalMargin,
                                           thisAccessorySize.height());
  }

  return k_topOffset + contentHeight + k_bottomOffset;
}

Responder *AbstractMenuCell::responder() {
  Responder *r = nullptr;
  /* The priority order was chose arbitrarely because there currently is no cell
   * with more than 1 responder. Accessory could be prioritized over subLabel or
   * label if needed. */
  (r = label()->responder()) || (r = subLabel()->responder()) ||
      (r = accessory()->responder());
  return r;
}

void AbstractMenuCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  label()->setHighlighted(highlight);
  subLabel()->setHighlighted(highlight);
  accessory()->setHighlighted(highlight);
}

int AbstractMenuCell::numberOfSubviews() const {
  return (labelView() != nullptr) + (subLabelView() != nullptr) +
         (accessoryView() != nullptr);
}

View *AbstractMenuCell::subviewAtIndex(int index) {
  if (index == 0) {
    return const_cast<View *>(labelView());
  }
  if (index == 1 && subLabelView() != nullptr) {
    return const_cast<View *>(subLabelView());
  }
  assert(index == 2 || (index == 1 && subLabelView() == nullptr));
  return const_cast<View *>(accessoryView());
}

void AbstractMenuCell::layoutSubviews(bool force) {
  /* Apply margins and separators on every side. At this point, we assume cell's
   * frame has been updated to add bottom and right overlapping borders. */
  // TODO : improve overlapping borders so that we don't need to assume that.
  KDCoordinate width = innerWidth();
  KDCoordinate height = innerHeight();
  if (width == 0 || height == 0) {
    return;
  }

  KDCoordinate y = k_topOffset;
  KDCoordinate x = k_leftOffset;
  KDCoordinate xEnd = x + width;

  KDSize thisLabelSize = labelSize();
  KDSize thisSubLabelSize = subLabelSize();
  KDSize thisAccessorySize = accessorySize();

  KDCoordinate labelHeight = std::min(thisLabelSize.height(), height);
  KDCoordinate labelWidth = std::min(thisLabelSize.width(), width);
  KDCoordinate subLabelHeight = std::min(thisSubLabelSize.height(), height);
  KDCoordinate subLabelWidth =
      shouldHideSublabel() ? 0 : std::min(thisSubLabelSize.width(), width);
  KDCoordinate accessoryHeight = std::min(thisAccessorySize.height(), height);
  KDCoordinate accessoryWidth = std::min(thisAccessorySize.width(), width);

  KDRect labelRect = KDRectZero, subLabelRect = KDRectZero,
         accessoryRect = KDRectZero;

  if (singleRowMode()) {  // Single row -> align vertically each view
    // Label on the left, aligned vertically
    labelRect =
        KDRect(x, y + (height - labelHeight) / 2, labelWidth, labelHeight);
    x += labelWidth + k_innerHorizontalMargin;

    if (shouldAlignSublabelRight()) {
      // Align SubLabel right
      x = xEnd - subLabelWidth;
      if (accessoryWidth > 0) {
        // Account for both accessoryWidth and the additional horizontal margin.
        x -= (accessoryWidth + k_innerHorizontalMargin);
      }
    }

    if (!shouldHideSublabel()) {
      subLabelRect = KDRect(x, y + (height - subLabelHeight) / 2, subLabelWidth,
                            subLabelHeight);
      x += subLabelWidth + k_innerHorizontalMargin;
    }

    KDCoordinate accessoryY = y + (height - accessoryHeight) / 2;
    if (accessoryIsAnEditableTextField()) {
      // Editable text field takes up all width
      accessoryWidth = xEnd - x;
    }
    accessoryRect = KDRect(xEnd - accessoryWidth, accessoryY, accessoryWidth,
                           accessoryHeight);
  } else {  // Two rows
    KDCoordinate firstRowHeight, firstColumnWidth, accessoryRowHeight;
    if (shouldAlignLabelAndAccessory()) {
      firstRowHeight = std::max(labelHeight, accessoryHeight);
      firstColumnWidth = labelWidth;
      accessoryRowHeight = firstRowHeight;
    } else {
      firstRowHeight = labelHeight;
      firstColumnWidth = std::max(labelWidth, subLabelWidth);
      accessoryRowHeight = height;
    }
    KDCoordinate accessoryX =
        accessoryIsAnEditableTextField()
            // Editable text field takes up all width
            ? k_leftOffset + k_innerHorizontalMargin + firstColumnWidth
            : xEnd - accessoryWidth;
    labelRect = KDRect(x, y + (firstRowHeight - labelHeight) / 2, labelWidth,
                       labelHeight);
    subLabelRect = KDRect(x, y + firstRowHeight + k_innerVerticalMargin,
                          subLabelWidth, subLabelHeight);
    accessoryRect =
        KDRect(accessoryX, y + (accessoryRowHeight - accessoryHeight) / 2,
               xEnd - accessoryX, accessoryHeight);
  }

  // Set frames
  setFrameIfViewExists(const_cast<View *>(labelView()), labelRect, force);
  setFrameIfViewExists(const_cast<View *>(subLabelView()), subLabelRect, force);
  setFrameIfViewExists(const_cast<View *>(accessoryView()), accessoryRect,
                       force);
}

bool AbstractMenuCell::shouldAlignLabelAndAccessory() const {
  if (forceAlignLabelAndAccessory()) {
    return true;
  }
  if (!subLabelView() || !accessoryView()) {
    return false;
  }
  KDCoordinate subLabelMinimalWidth = subLabelSize().width();
  KDCoordinate minimalCumulatedWidthOfAccessoryAndSubLabel =
      subLabelMinimalWidth + k_innerHorizontalMargin + accessorySize().width();
  return minimalCumulatedWidthOfAccessoryAndSubLabel > innerWidth();
}

bool AbstractMenuCell::shouldHideSublabel() const {
  return accessoryIsAnEditableTextField() && singleRowMode() &&
         static_cast<const EditableTextWidget *>(constAccessory())->isEditing();
}

bool AbstractMenuCell::singleRowMode() const {
  KDSize thisLabelSize = labelSize();
  KDSize thisSubLabelSize = subLabelSize();
  KDSize thisAccessorySize = accessorySize();
  KDCoordinate accessoryWidth =
      accessoryIsAnEditableTextField()
          ? std::max(thisAccessorySize.width(),
                     static_cast<const EditableTextWidget *>(constAccessory())
                         ->minimalWidth())
          : thisAccessorySize.width();
  KDCoordinate thisInnerWidth = innerWidth();

  KDCoordinate minimalWidth = thisLabelSize.width() + k_innerHorizontalMargin +
                              thisSubLabelSize.width();
  if (accessoryWidth > 0) {
    minimalWidth += k_innerHorizontalMargin + accessoryWidth;
  }

  return (minimalWidth < thisInnerWidth) || thisLabelSize.width() == 0 ||
         thisSubLabelSize.width() == 0;
}

KDRect AbstractMenuCell::setFrameIfViewExists(View *v, KDRect rect,
                                              bool force) {
  if (v) {
    setChildFrame(v, rect, force);
    return rect;
  }
  return KDRectZero;
}

}  // namespace Escher
