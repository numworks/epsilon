#ifndef SETTINGS_GENERIC_SUB_CONTROLLER_H
#define SETTINGS_GENERIC_SUB_CONTROLLER_H

#include <escher/message_tree.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

namespace Settings {

class GenericSubController : public Escher::SelectableListViewController<
                                 Escher::MemoizedListViewDataSource> {
 public:
  GenericSubController(Escher::Responder* parentResponder);
  const char* title() override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  void setMessageTreeModel(const Escher::MessageTree* messageTreeModel);
  void viewDidDisappear() override;

 protected:
  Escher::StackViewController* stackController() const;
  virtual int initialSelectedRow() const { return 0; }
  Escher::MessageTree* m_messageTreeModel;
};

}  // namespace Settings

#endif
