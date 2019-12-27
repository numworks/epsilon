#include <escher/table_cell.h>
#include <escher/palette.h>
#include <escher/metric.h>

static inline KDCoordinate minCoordinate(KDCoordinate x, KDCoordinate y) { return x < y ? x : y; }

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

void TableCell::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  View * label = labelView();
  KDSize labelSize = label ? label->minimalSizeForOptimalDisplay() : KDSizeZero;
  if (label) {
    switch (m_layout) {
      case Layout::Vertical:
      {
        KDCoordinate x = k_separatorThickness+labelMargin();
        KDCoordinate y = k_separatorThickness+Metric::TableCellLabelTopMargin;
        label->setFrame(KDRect(
              x,
              y,
              width-2*x,
              minCoordinate(labelSize.height(), height-2*y)),
        force);
        break;
      }
      default:
      {
        KDCoordinate x = k_separatorThickness+labelMargin();
        KDCoordinate y = k_separatorThickness;
        label->setFrame(KDRect(
              x,
              k_separatorThickness,
              minCoordinate(labelSize.width(), width-2*x),
              height - 2*y),
        force);
        break;
      }
    }
  }
  View * accessory = accessoryView();
  if (accessory) {
    KDSize accessorySize = accessory->minimalSizeForOptimalDisplay();
    switch (m_layout) {
      case Layout::Vertical:
      {
        KDCoordinate x = k_separatorThickness+k_accessoryMargin;
        accessory->setFrame(KDRect(
              x,
              height-k_separatorThickness-accessorySize.height()-k_accessoryBottomMargin,
              width-2*x,
              accessorySize.height()),
        force);
        break;
      }
      default:
        // In some cases, the accessory view cannot take all the size it can
        KDCoordinate wantedX = width-accessorySize.width()-k_separatorThickness-k_accessoryMargin;
        KDCoordinate minX = label ? label->bounds().x()+labelSize.width()+labelMargin()+k_separatorThickness+k_accessoryMargin : k_accessoryMargin;
        KDCoordinate x = minX < wantedX ? wantedX : minX;
        accessory->setFrame(KDRect(
            x,
            k_separatorThickness,
            minCoordinate(accessorySize.width(), width - x),
            height-2*k_separatorThickness),
          force);
        break;
    }
  }
  View * subAccessory = subAccessoryView();
  if (subAccessory && accessory) {
    KDSize accessorySize = accessory->minimalSizeForOptimalDisplay();
    KDSize subAccessorySize = subAccessory->minimalSizeForOptimalDisplay();
    subAccessory->setFrame(KDRect(width-k_separatorThickness-k_accessoryMargin-accessorySize.width()-subAccessorySize.width(), k_separatorThickness,
      subAccessorySize.width(), height-2*k_separatorThickness), force);
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
