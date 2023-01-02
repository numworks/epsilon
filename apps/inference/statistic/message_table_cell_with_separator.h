#ifndef INFERENCE_STATISTIC_MESSAGE_TABLE_CELL_WITH_SEPARATOR_H
#define INFERENCE_STATISTIC_MESSAGE_TABLE_CELL_WITH_SEPARATOR_H

#include <escher/message_table_cell_with_editable_text_with_message.h>

namespace Inference {

class MessageTableCellWithSeparator : public Escher::TableCell, public Escher::Responder {
public:
  MessageTableCellWithSeparator(
      Escher::Responder * parentResponder = nullptr,
      Escher::InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr,
      Escher::TextFieldDelegate * textFieldDelegate = nullptr,
      I18n::Message message = (I18n::Message)0);
  Escher::Responder * responder() override { return &m_innerCell; }
  void drawRect(KDContext * ctx, KDRect rect) const override;
  int numberOfSubviews() const override { return 1; }
  Escher::View * subviewAtIndex(int i) override { return &m_innerCell; }
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force = false) override;

  void setHighlighted(bool highlight) override;
  Escher::MessageTableCellWithEditableTextWithMessage * innerCell() { return &m_innerCell; }

private:
  constexpr static KDCoordinate k_margin = Escher::Metric::CommonMenuMargin;
  constexpr static KDCoordinate k_lineThickness = Escher::Metric::CellSeparatorThickness;
  mutable Escher::MessageTableCellWithEditableTextWithMessage m_innerCell;
};

}

#endif
