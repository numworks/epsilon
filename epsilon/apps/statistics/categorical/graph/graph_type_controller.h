#include <apps/i18n.h>
#include <escher/alternate_empty_view_delegate.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/tab_view_controller.h>
#include <escher/transparent_image_view.h>

namespace Statistics {

using GraphTypeCell =
    Escher::MenuCell<Escher::TransparentImageView, Escher::MessageTextView>;

class CategoricalGraphTypeController
    : public Escher::AlternateEmptyViewDelegate,
      public Escher::UniformSelectableListController<GraphTypeCell, 2> {
 public:
  CategoricalGraphTypeController(Escher::Responder* parentResponder,
                                 Escher::TabViewController* tabViewController)
      : Escher::UniformSelectableListController<GraphTypeCell, k_numberOfCells>(
            parentResponder),
        m_tabController(tabViewController) {};

  // TEMP: AlternateEmptyViewDelegate
  bool isEmpty() const override { return true; }
  I18n::Message emptyMessage() override { return I18n::Message::Categorical; }
  Escher::Responder* responderWhenEmpty() override {
    m_tabController->selectTab();
    return m_tabController;
  }

  Escher::TabViewController* m_tabController;
};

}  // namespace Statistics
