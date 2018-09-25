#ifndef SETTINGS_GENERIC_SUB_CONTROLLER_H
#define SETTINGS_GENERIC_SUB_CONTROLLER_H

#include <escher.h>
#include "../settings_message_tree.h"

namespace Settings {

class GenericSubController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  GenericSubController(Responder * parentResponder);
  const char * title() override;
  View * view() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void setMessageTreeModel(const MessageTree * messageTreeModel);
  void viewWillAppear() override;
  void viewDidDisappear() override;
protected:
  StackViewController * stackController() const;
  constexpr static KDCoordinate k_topBottomMargin = 13;
  SelectableTableView m_selectableTableView;
  MessageTree * m_messageTreeModel;
};

}

#endif
