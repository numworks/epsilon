#ifndef PROBABILITY_ABSTRACT_INPUT_HOMOGENEITY_VIEW_H
#define PROBABILITY_ABSTRACT_INPUT_HOMOGENEITY_VIEW_H

#include "input_categorical_view.h"

namespace Probability {

/* This view contains a pointer to a TableView, an EditableCell and a Button,
 * laid out vertically, and is able to move selection between them.
 */
class InputHomogeneityView : public InputCategoricalView {
public:
  InputHomogeneityView(Escher::Responder * parentResponder,
                    Escher::Invocation invocation,
                    TableViewController * tableViewController,
                    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                    Escher::TextFieldDelegate * textFieldDelegate);

  constexpr static int k_indexOfSignificance = 1;
  constexpr static int k_indexOfSpacer = 2;
  constexpr static int k_indexOfNext = 3;

  int indexOfSignificance() override { return k_indexOfSignificance; }
  int indexOfSpacer() override { return k_indexOfSpacer; }
  int indexOfNext() override { return k_indexOfNext; }
private:
  /* Layout a Table, a cell and a button separated by spacers. */
  class HomogeneityContentView : public InputCategoricalView::ContentView {
  public:
    HomogeneityContentView(Escher::SelectableTableView * dataInputTableView,
                Escher::MessageTableCellWithEditableTextWithMessage * significanceCell,
                Escher::Button * next);

  private:
    /* Layout cell and button with side margins */
    class InnerVerticalLayout : public Escher::VerticalLayout {
    public:
      InnerVerticalLayout(Escher::MessageTableCellWithEditableTextWithMessage * significanceCell,
                          Escher::Button * next) :
            VerticalLayout(Escher::Palette::WallScreenDark),
            m_significanceCell(significanceCell),
            m_next(next) {
        setSecondaryDirectionMargin(Escher::Metric::CommonMargin);
      };
      Escher::View * subviewAtIndex(int i) override;
      int numberOfSubviews() const override { return 2; }

    private:
      Escher::MessageTableCellWithEditableTextWithMessage * m_significanceCell;
      Escher::Button * m_next;
    };
    Escher::VerticalLayout * innerView() override { return &m_innerView; }

    InnerVerticalLayout m_innerView;
  };

  void highlightCorrectView() override;
  InputCategoricalView::ContentView * contentView() override { return &m_contentView; };
  InputCategoricalView::ContentView * constContentView() const override { return const_cast<HomogeneityContentView *>(&m_contentView); };

  HomogeneityContentView m_contentView;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_INPUT_HOMOGENEITY_VIEW_H */
