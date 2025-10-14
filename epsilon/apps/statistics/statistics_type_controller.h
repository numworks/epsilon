#pragma once

#include <apps/i18n.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include "statistics_type_view_model.h"

namespace Statistics {

using DataTypeCell =
    Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView>;

/* Controller for the statistics data type menu.
 * Allows changing between quantitative and categorical data type */
class DataTypeController
    : public Escher::UniformSelectableListController<
          DataTypeCell, DataTypeViewModel::k_numberOfDataTypes> {
 public:
  DataTypeController(Escher::Responder* parentResponder,
                     Escher::StackViewController* stackView,
                     DataTypeViewModel* dataTypeModel);

  // UniformSelectableListController
  bool handleEvent(Ion::Events::Event event) override;
  const char* title() const override {
    return I18n::translate(I18n::Message::DataType);
  }

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  Escher::StackViewController* m_stackViewController;
  DataTypeViewModel* m_dataTypeModel;
};

}  // namespace Statistics
