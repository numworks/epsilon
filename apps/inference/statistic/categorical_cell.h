#ifndef INFERENCE_CATEGORICAL_CELL_H
#define INFERENCE_CATEGORICAL_CELL_H

#include <apps/i18n.h>
#include <escher/button_cell.h>
#include <escher/dropdown_widget.h>
#include <escher/menu_cell_with_editable_text.h>

namespace Inference {

// Cell with left and right margins for InputCategoricalController

class AbstractCategoricalCell : public Escher::HighlightCell {
 public:
  using HighlightCell::HighlightCell;

  // View
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext* ctx, KDRect rect) const override;

  // HighlightCell
  void setHighlighted(bool highlight) override;
  Escher::Responder* responder() override { return innerCell()->responder(); }
  const char* text() const override { return innerCell()->text(); }
  Poincare::Layout layout() const override { return innerCell()->layout(); }
  void initSize(KDCoordinate width) override {
    return innerCell()->initSize(width - Escher::Metric::CommonMargins.width());
  }

  void setVisible(bool visible) override {
    innerCell()->setVisible(visible);
    Escher::HighlightCell::setVisible(visible);
  }

 private:
  // View
  int numberOfSubviews() const override { return 1; }
  HighlightCell* subviewAtIndex(int index) override = 0;
  void layoutSubviews(bool force) override;

  HighlightCell* innerCell() { return subviewAtIndex(0); }
  const HighlightCell* innerCell() const {
    return const_cast<AbstractCategoricalCell*>(this)->innerCell();
  }
};

template <typename T>
class InputCategoricalCell : public AbstractCategoricalCell {
 public:
  InputCategoricalCell(Escher::Responder* parent = nullptr,
                       Escher::TextFieldDelegate* textFieldDelegate = nullptr)
      : AbstractCategoricalCell(), m_innerCell(parent, textFieldDelegate) {}

  Escher::TextField* textField() { return m_innerCell.textField(); }
  void setMessages(typename T::TitleType labelMessage,
                   I18n::Message subLabelMessage = I18n::Message::Default);

 private:
  HighlightCell* subviewAtIndex(int i) override { return &m_innerCell; }

  Escher::MenuCellWithEditableText<T, Escher::MessageTextView> m_innerCell;
};

class DropdownCategoricalCell : public AbstractCategoricalCell {
 public:
  DropdownCategoricalCell(
      Escher::Responder* parent,
      Escher::ExplicitListViewDataSource* dropdownDataSource,
      Escher::DropdownCallback* callback)
      : m_dropdown(parent, dropdownDataSource, callback) {
    m_innerCell.accessory()->setDropdown(&m_dropdown);
  }

  void setMessage(I18n::Message message) {
    m_innerCell.label()->setMessage(message);
  }
  Escher::Dropdown* dropdown() { return &m_dropdown; }

 private:
  HighlightCell* subviewAtIndex(int i) override { return &m_innerCell; }

  /* WARNING: make sure the Dropdown DataSource is constructed before the
   * Dropdown. */
  Escher::Dropdown m_dropdown;
  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::DropdownWidget>
      m_innerCell;
};

class ButtonCategoricalCell : public AbstractCategoricalCell {
 public:
  ButtonCategoricalCell(Escher::Responder* parent, I18n::Message message,
                        Escher::Invocation invocation)
      : m_innerCell(parent, message, invocation,
                    Escher::ButtonCell::Style::EmbossedLight) {}

 private:
  HighlightCell* subviewAtIndex(int i) override { return &m_innerCell; }

  Escher::ButtonCell m_innerCell;
};

}  // namespace Inference

#endif
