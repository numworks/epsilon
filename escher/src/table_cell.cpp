#include <escher/table_cell.h>
#include <escher/palette.h>
#include <escher/metric.h>

TableCell::TableCell(Layout layout) :
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

void TableCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  View * label = labelView();
  KDSize labelSize = label ? label->minimalSizeForOptimalDisplay() : KDSizeZero;
  if (label) {
    switch (m_layout) {
      case Layout::Vertical:
        label->setFrame(KDRect(
              k_separatorThickness+k_labelMargin,
              k_separatorThickness+Metric::TableCellLabelTopMargin,
              width-2*k_separatorThickness-k_labelMargin,
              labelSize.height()));
        break;
      default:
        label->setFrame(KDRect(
              k_separatorThickness+k_labelMargin,
              k_separatorThickness,
              labelSize.width(),
              height - 2*k_separatorThickness));
        break;
    }
  }
  View * accessory = accessoryView();
  if (accessory) {
    KDSize accessorySize = accessory->minimalSizeForOptimalDisplay();
    switch (m_layout) {
      case Layout::Vertical:
        accessory->setFrame(KDRect(
              k_separatorThickness+k_accessoryMargin,
              height-k_separatorThickness-accessorySize.height()-k_accessoryBottomMargin,
              width-2*k_separatorThickness - k_accessoryMargin,
              accessorySize.height()));
        break;
      default:
        // In some cases, the accessory view cannot take all the size it can
        KDCoordinate wantedX = width-accessorySize.width()-k_separatorThickness-k_accessoryMargin;
        KDCoordinate minX = label ? label->bounds().x()+labelSize.width()+k_labelMargin+k_separatorThickness+k_accessoryMargin : k_accessoryMargin;
        if (minX < wantedX) {
          accessory->setFrame(KDRect(
                wantedX,
                k_separatorThickness,
                accessorySize.width(),
                height-2*k_separatorThickness));
        } else {
          accessory->setFrame(KDRect(
                minX,
                k_separatorThickness,
                accessorySize.width(),
                height-2*k_separatorThickness));
        }
        break;
    }
  }
  View * subAccessory = subAccessoryView();
  if (subAccessory && accessory) {
    KDSize accessorySize = accessory->minimalSizeForOptimalDisplay();
    KDSize subAccessorySize = subAccessory->minimalSizeForOptimalDisplay();
    subAccessory->setFrame(KDRect(width-k_separatorThickness-k_accessoryMargin-accessorySize.width()-subAccessorySize.width(), k_separatorThickness,
      subAccessorySize.width(), height-2*k_separatorThickness));
  }
}

void TableCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;
  ctx->fillRect(KDRect(k_separatorThickness, k_separatorThickness, width-2*k_separatorThickness, height-k_separatorThickness), backgroundColor);
  // Draw rectangle around cell
  ctx->fillRect(KDRect(0, 0, width, k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(0, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(width-k_separatorThickness, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(0, height-k_separatorThickness, width, k_separatorThickness), Palette::GreyBright);
 }
