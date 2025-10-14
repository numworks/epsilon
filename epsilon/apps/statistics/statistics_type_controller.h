#pragma once

#include <apps/i18n.h>
#include <escher/alternate_empty_view_delegate.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>
#include <escher/tab_view_data_source.h>
#include <escher/transparent_image_view.h>

#include "escher/alternate_view_controller.h"
#include "statistics_type_view_model.h"
#include "store.h"

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
