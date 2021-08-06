#ifndef APPS_PROBABILITY_ABSTRACT_TABLE_VIEW_CONTROLLER_H
#define APPS_PROBABILITY_ABSTRACT_TABLE_VIEW_CONTROLLER_H

#include <apps/shared/text_field_delegate.h>
#include <escher/metric.h>
#include <escher/selectable_table_view.h>
#include <ion/events.h>

namespace Probability {

class TableViewController : public Escher::Responder, public Shared::TextFieldDelegate {
public:
  using Escher::Responder::Responder;

  void didBecomeFirstResponder() override;

  virtual Escher::SelectableTableView * selectableTableView() = 0;
  virtual Escher::TableViewDataSource * tableViewDataSource() = 0;

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::TextField * textField,
                                    Ion::Events::Event event) override;

protected:
  void moveSelectionForEvent(Ion::Events::Event event, int * selectedRow, int * selectedColumn);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_ABSTRACT_TABLE_VIEW_CONTROLLER_H */
