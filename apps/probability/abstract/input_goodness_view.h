#ifndef PROBABILITY_ABSTRACT_INPUT_GOODNESS_VIEW_H
#define PROBABILITY_ABSTRACT_INPUT_GOODNESS_VIEW_H

#include "input_categorical_view.h"

namespace Probability {

/* This view contains a pointer to a TableView, two EditableCells and a Button,
 * laid out vertically, and is able to move selection between them.
 */
class InputGoodnessView : public InputCategoricalView {
public:
  InputGoodnessView(Escher::Responder * parentResponder,
                    Escher::Invocation invocation,
                    TableViewController * tableViewController,
                    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                    Escher::TextFieldDelegate * textFieldDelegate);

  void updateDegreeOfFreedomCell(int degreeOfFreedom);

  constexpr static int k_indexOfDegreeOfFreedom = 1;
  constexpr static int k_indexOfSignificance = 2;
  constexpr static int k_indexOfSpacer = 3;
  constexpr static int k_indexOfNext = 4;

  int indexOfSignificance() override { return k_indexOfSignificance; }
  int indexOfSpacer() override { return k_indexOfSpacer; }
  int indexOfNext() override { return k_indexOfNext; }

private:
  /* Layout a Table, two cells and a button separated by spacers. */
  class GoodnessContentView : public InputCategoricalView::ContentView {
  public:
    GoodnessContentView(Escher::SelectableTableView * dataInputTableView,
                Escher::MessageTableCellWithEditableTextWithMessage * degreeOfFreedomCell,
                Escher::MessageTableCellWithEditableTextWithMessage * significanceCell,
                Escher::Button * next);

  private:
    /* Layout cell and button with side margins */
    class InnerVerticalLayout : public Escher::VerticalLayout {
    public:
      InnerVerticalLayout(Escher::MessageTableCellWithEditableTextWithMessage * degreeOfFreedomCell,
                          Escher::MessageTableCellWithEditableTextWithMessage * significanceCell,
                          Escher::Button * next) :
            VerticalLayout(Escher::Palette::WallScreenDark),
            m_degreeOfFreedomCell(degreeOfFreedomCell),
            m_significanceCell(significanceCell),
            m_next(next) {
        // TODO : Fix the margin between these two cells.
        setSecondaryDirectionMargin(Escher::Metric::CommonMargin);
      };
      Escher::View * subviewAtIndex(int i) override;
      int numberOfSubviews() const override { return 3; }

    private:
      Escher::MessageTableCellWithEditableTextWithMessage * m_degreeOfFreedomCell;
      Escher::MessageTableCellWithEditableTextWithMessage * m_significanceCell;
      Escher::Button * m_next;
    };
    Escher::VerticalLayout * innerView() override { return &m_innerView; }

    InnerVerticalLayout m_innerView;
  };

  Responder * responderForRow(int row) override;
  void highlightCorrectView() override;
  InputCategoricalView::ContentView * contentView() override { return &m_contentView; };
  InputCategoricalView::ContentView * constContentView() const override { return const_cast<GoodnessContentView *>(&m_contentView); };

  Escher::MessageTableCellWithEditableTextWithMessage m_degreeOfFreedomCell;
  GoodnessContentView m_contentView;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_INPUT_GOODNESS_VIEW_H */
