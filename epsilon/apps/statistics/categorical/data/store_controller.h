
#include <apps/i18n.h>
#include <escher/alternate_empty_view_delegate.h>
#include <escher/button_row_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>

namespace Statistics {

class CategoricalStoreController : public Escher::ViewController,
                                   public Escher::AlternateEmptyViewDelegate,
                                   public Escher::ButtonRowDelegate {
 public:
  CategoricalStoreController(Escher::Responder* parentResponder,
                             Escher::ButtonRowController* header,
                             Escher::StackViewController* stackViewController,
                             Escher::TabViewController* tabViewController,
                             Escher::ViewController* dataTypeController)
      : Escher::ViewController(parentResponder),
        Escher::ButtonRowDelegate(header, nullptr),
        m_stackViewController(stackViewController),
        m_tabController(tabViewController),
        m_dataTypeController(dataTypeController),
        m_dataTypeButton(
            this, I18n::Message::DataType,
            Escher::Invocation::Builder<CategoricalStoreController>(
                [](CategoricalStoreController* controller, void* sender) {
                  controller->pushTypeController();
                  return true;
                },
                this),
            KDFont::Size::Small) {}

  // Escher::ButtonRowDelegate
  int numberOfButtons(
      Escher::ButtonRowController::Position position) const override {
    assert(position == Escher::ButtonRowController::Position::Top);
    return 1;
  }
  Escher::ButtonCell* buttonAtIndex(
      int index,
      Escher::ButtonRowController::Position position) const override {
    assert(index == 0 &&
           position == Escher::ButtonRowController::Position::Top);
    return const_cast<Escher::SimpleButtonCell*>(&m_dataTypeButton);
  }

  Escher::View* view() override { OMG::unreachable(); }

  void pushTypeController() {
    m_stackViewController->push(m_dataTypeController);
  }

  void handleResponderChainEvent(
      Responder::ResponderChainEvent event) override {
    if (event.type == ResponderChainEventType::HasBecomeFirst) {
      header()->setSelectedButton(0);
    }
  }

  bool handleEvent(Ion::Events::Event event) override {
    if (event == Ion::Events::Up) {
      if (header()->selectedButton() >= 0) {
        header()->setSelectedButton(-1);
        m_tabController->selectTab();
      } else {
        header()->setSelectedButton(0);
      }
      return true;
    }
    return false;
  }

  // TEMP: AlternateEmptyViewDelegate
  bool isEmpty() const override { return true; }
  I18n::Message emptyMessage() override { return I18n::Message::Categorical; }
  Escher::Responder* responderWhenEmpty() override { return this; }

  Escher::StackViewController* m_stackViewController;
  Escher::TabViewController* m_tabController;
  Escher::ViewController* m_dataTypeController;
  Escher::SimpleButtonCell m_dataTypeButton;
};

}  // namespace Statistics
