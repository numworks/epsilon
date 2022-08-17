#ifndef SHARED_MENU_CONTROLLER_H
#define SHARED_MENU_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/subapp_cell.h>
#include <escher/view_controller.h>

namespace Shared {

constexpr int k_menuControllerMaxNumberOfCells = 3;

class MenuControllerDelegate {
public:
  virtual void selectSubApp(int subAppIndex) = 0;
  virtual int selectedSubApp() const = 0;
  virtual int numberOfSubApps() const = 0;
};

class MenuController : public Escher::SelectableCellListPage<Escher::SubappCell, k_menuControllerMaxNumberOfCells, Escher::MemoizedListViewDataSource> {
public:
  MenuController(Escher::StackViewController * parentResponder, std::initializer_list<Escher::ViewController *> controllers, std::initializer_list<std::initializer_list<I18n::Message>> messages, std::initializer_list<const Escher::Image *> images, MenuControllerDelegate * delegate);
  void stackOpenPage(Escher::ViewController * nextPage) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return m_delegate->numberOfSubApps(); }

private:
  Escher::ViewController * m_controllers[k_menuControllerMaxNumberOfCells];
  MenuControllerDelegate * m_delegate;
};

}  // namespace Shared

#endif /* SHARED_MENU_CONTROLLER_H */

