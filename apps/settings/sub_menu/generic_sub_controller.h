#ifndef SETTINGS_GENERIC_SUB_CONTROLLER_H
#define SETTINGS_GENERIC_SUB_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/simple_list_view_data_source.h>
#include <escher/stack_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <apps/shared/settings_message_tree.h>

namespace Settings {

class GenericSubController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  GenericSubController(Escher::Responder * parentResponder);
  const char * title() override;
  Escher::View * view() override { return &m_selectableTableView; }
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  KDCoordinate cellWidth() override {
    if (m_selectableTableView.columnWidth(0) <= 0) {
      return 320; // TODO : FIx this
    }
    return m_selectableTableView.columnWidth(0);
  }
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void setMessageTreeModel(const Escher::MessageTree * messageTreeModel);
  void viewDidDisappear() override;
protected:
  Escher::StackViewController * stackController() const;
  virtual int initialSelectedRow() const { return 0; }
  constexpr static KDCoordinate k_topBottomMargin = 13;
  Escher::SelectableTableView m_selectableTableView;
  Escher::MessageTree * m_messageTreeModel;
};

}

#endif
