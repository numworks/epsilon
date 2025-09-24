#pragma once

#include <apps/i18n.h>
#include <apps/shared/sequence.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <omg/expiring_pointer.h>

namespace Sequence {

class NotationParameterController : public Escher::SelectableListViewController<
                                        Escher::SimpleListViewDataSource> {
 public:
  NotationParameterController(Escher::Responder* parentResponder)
      : SelectableListViewController<SimpleListViewDataSource>(
            parentResponder) {}

  // ViewController
  const char* title() const override {
    return I18n::translate(I18n::Message::SequenceNotation);
  }
  void viewWillAppear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  // SimpleListViewDataSource
  Escher::HighlightCell* reusableCell(int index) override {
    return &m_cells[index];
  }
  int reusableCellCount() const override { return k_numberOfCells; }
  int numberOfRows() const override { return k_numberOfCells; }
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

  // NotationParameterController
  void setRecord(Ion::Storage::Record record);

 private:
  constexpr static int k_numberOfCells = 2;
  constexpr static int k_defaultNotationRow = 0;
  constexpr static int k_shiftedNotationRow = 1;

  using Notation = Shared::Sequence::RecursiveNotation;
  int rowForNotation(Notation notation) const {
    return notation == Notation::Default ? k_defaultNotationRow
                                         : k_shiftedNotationRow;
  }
  Notation notationForRow(int row) const {
    return row == k_defaultNotationRow ? Notation::Default : Notation::Shifted;
  }

  OMG::ExpiringPointer<Shared::Sequence> sequence();

  Ion::Storage::Record m_record;
  Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView>
      m_cells[k_numberOfCells];
};

}  // namespace Sequence
