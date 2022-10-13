#ifndef PERDIODIC_PHYSICAL_QUANTITY_CELL_H
#define PERDIODIC_PHYSICAL_QUANTITY_CELL_H

#include <apps/shared/cell_with_separator.h>
#include <apps/i18n.h>
#include <escher/expression_view.h>
#include <escher/message_text_view.h>
#include <escher/table_cell.h>

namespace Elements {

/* Table cell with two layouts and a message sublabel */
class PhysicalQuantityCell : public Escher::TableCell {
public:
  PhysicalQuantityCell();

  const Escher::View * labelView() const override { return &m_labelView; }
  const Escher::View * subLabelView() const override { return &m_subLabelView; }
  const Escher::View * accessoryView() const override { return &m_accessoryView; }
  void setHighlighted(bool highlight) override;
  bool forceAlignLabelAndAccessory() const override { return true; }
  bool shouldAlignSublabelRight() const override { return false; }

  void setLayouts(Poincare::Layout label, Poincare::Layout accessory);
  void setSubLabelMessage(I18n::Message message) { m_subLabelView.setMessage(message); }
  void setLabelColor(KDColor color) { m_labelView.setTextColor(color); }
  void setAccessoryColor(KDColor color) { m_accessoryView.setTextColor(color); }

private:
  Escher::ExpressionView m_labelView;
  Escher::MessageTextView m_subLabelView;
  Escher::ExpressionView m_accessoryView;
};

class PhysicalQuantityCellWithSeparator : public Shared::CellWithSeparator {
public:
  PhysicalQuantityCell * innerCell() { return &m_cell; }

private:
  const Escher::TableCell * constCell() const override { return &m_cell; }

  PhysicalQuantityCell m_cell;
};

}

#endif
