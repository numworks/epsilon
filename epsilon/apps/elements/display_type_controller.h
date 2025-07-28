#ifndef ELEMENTS_DISPLAY_TYPE_CONTROLLER_H
#define ELEMENTS_DISPLAY_TYPE_CONTROLLER_H

#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include "elements_view_data_source.h"

namespace Elements {

class DisplayTypeController
    : public Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView>, 8> {
 public:
  DisplayTypeController(Escher::StackViewController* stackController);
  bool handleEvent(Ion::Events::Event e) override;
  const char* title() const override {
    return I18n::translate(I18n::Message::DisplayTypeTitle);
  }
  void viewWillAppear() override;

 private:
  constexpr static const DataField* k_fields[k_numberOfCells] = {
      &ElementsDataBase::GroupField,
      &ElementsDataBase::BlockField,
      &ElementsDataBase::MetalField,
      &ElementsDataBase::MassField,
      &ElementsDataBase::ElectronegativityField,
      &ElementsDataBase::MeltingPointField,
      &ElementsDataBase::BoilingPointField,
      &ElementsDataBase::RadiusField,
  };

  Escher::StackViewController* stackViewController() const {
    return static_cast<Escher::StackViewController*>(parentResponder());
  }
};

}  // namespace Elements

#endif
