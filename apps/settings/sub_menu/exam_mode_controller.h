#ifndef SETTINGS_EXAM_MODE_CONTROLLER_H
#define SETTINGS_EXAM_MODE_CONTROLLER_H

#include "generic_sub_controller.h"
#include "../../global_preferences.h"

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
    static constexpr int k_maxNumberOfLines = 6;
    static constexpr int k_numberOfDeactivationMessageLines = 3;
    void setMessages(I18n::Message m[k_maxNumberOfLines]);
    int numberOfCautionLines() const;
  private:
    int numberOfMessages() const;
    int numberOfSubviews() const override { return 1 + numberOfMessages(); }
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    SelectableTableView * m_selectableTableView;
    MessageTextView m_messageLines[k_maxNumberOfLines];
  };
  int initialSelectedRow() const override;
  GlobalPreferences::ExamMode examMode();
  static constexpr int k_maxNumberOfCells = 2;
  ContentView m_contentView;
  MessageTableCell m_cell[k_maxNumberOfCells];
};

}

#endif
