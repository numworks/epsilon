#ifndef SETTINGS_EXAM_MODE_CONTROLLER_H
#define SETTINGS_EXAM_MODE_CONTROLLER_H

#include "generic_sub_controller.h"

namespace Settings {

class ExamModeController : public GenericSubController {
public:
  ExamModeController(Responder * parentResponder);
  View * view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  int numberOfRows() const override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  class ContentView : public View {
  public:
    ContentView(SelectableTableView * selectableTableView);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void layoutSubviews(bool force = false) override;
  private:
    bool shouldDisplayDeactivateMessages() const;
    int numberOfSubviews() const override { return 1 + (shouldDisplayDeactivateMessages() ? 3 : 0); }
    View * subviewAtIndex(int index) override;
    SelectableTableView * m_selectableTableView;
    MessageTextView m_deactivateLine1;
    MessageTextView m_deactivateLine2;
    MessageTextView m_deactivateLine3;
  };
  static constexpr int k_maxNumberOfCells = 2;
  ContentView m_contentView;
  MessageTableCell m_cell[k_maxNumberOfCells];
};

}

#endif
