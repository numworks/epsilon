#pragma once

#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view.h>
#include <escher/switch_view.h>
#include <escher/view.h>

#include "generic_sub_controller.h"

namespace Settings {

class AccessibilityController : public GenericSubController {
 public:
  AccessibilityController(Escher::Responder* parentResponder);
  Escher::View* view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

 private:
  constexpr static int k_totalNumberOfCell = 2;
  constexpr static int k_contrastRow = 0;
  constexpr static int k_magnifyRow = 1;

  using SwitchCell =
      Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                       Escher::SwitchView>;

  class ContentView : public Escher::View {
   public:
    ContentView(Escher::SelectableListView* listView);
    void drawRect(KDContext* ctx, KDRect rect) const override;

   private:
    int numberOfSubviews() const override { return 2; }
    Escher::View* subviewAtIndex(int i) override;
    void layoutSubviews(bool force = false) override;

    Escher::SelectableListView* m_listView;
    Escher::MessageTextView m_hintMessage;
  };

  SwitchCell m_switchCells[k_totalNumberOfCell];
  ContentView m_contentView;
};

}  // namespace Settings
