#ifndef SHARED_MENU_CONTROLLER_H
#define SHARED_MENU_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/subapp_cell.h>
#include <escher/view_controller.h>

namespace Shared {

class MenuControllerDelegate {
 public:
  virtual void selectSubApp(int subAppIndex) = 0;
  virtual int selectedSubApp() const = 0;
  virtual int numberOfSubApps() const = 0;
};

class MenuController
    : public Escher::UniformSelectableListController<Escher::SubappCell, 2> {
 public:
  MenuController(
      Escher::StackViewController* parentResponder,
      std::initializer_list<Escher::ViewController*> controllers,
      std::initializer_list<std::initializer_list<I18n::Message>> messages,
      std::initializer_list<const Escher::Image*> images,
      MenuControllerDelegate* delegate);
  void stackOpenPage(Escher::ViewController* nextPage) override;
  bool handleEvent(Ion::Events::Event event) override;

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  Escher::ViewController* m_controllers[k_numberOfCells];
  MenuControllerDelegate* m_delegate;
};

}  // namespace Shared

#endif
