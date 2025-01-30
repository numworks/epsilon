#ifndef SEQUENCE_TYPE_PARAMETER_CONTROLLER_H
#define SEQUENCE_TYPE_PARAMETER_CONTROLLER_H

#include <apps/shared/sequence_store.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/scrollable_layout_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

namespace Sequence {

class ListController;

class TypeParameterController
    : public Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView>, 3> {
 public:
  TypeParameterController(Escher::Responder* parentResponder,
                          ListController* list, KDMargins margins = {});
  const char* title() const override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;
  void setRecord(Ion::Storage::Record record) { m_record = record; }

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  constexpr static int k_indexOfExplicit =
      static_cast<int>(Shared::Sequence::Type::Explicit);
  constexpr static int k_indexOfRecurrence =
      static_cast<int>(Shared::Sequence::Type::SingleRecurrence);
  constexpr static int k_indexOfDoubleRecurrence =
      static_cast<int>(Shared::Sequence::Type::DoubleRecurrence);

  Escher::StackViewController* stackController() const {
    return static_cast<Escher::StackViewController*>(parentResponder());
  }
  Shared::Sequence* sequence() {
    assert(!isNewModel());
    return sequenceStore()->modelForRecord(m_record);
  }
  Shared::SequenceStore* sequenceStore();
  bool isNewModel() const { return m_record.isNull(); }

  Ion::Storage::Record m_record;
  ListController* m_listController;
};

}  // namespace Sequence

#endif
