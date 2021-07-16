#ifndef APPS_PROBABILITY_ABSTRACT_TABLE_VIEW_CONTROLLER_H
#define APPS_PROBABILITY_ABSTRACT_TABLE_VIEW_CONTROLLER_H

#include <apps/shared/text_field_delegate.h>
#include <escher/metric.h>
#include <escher/selectable_table_view.h>

namespace Probability {

class TableViewController : public Escher::Responder, public Shared::TextFieldDelegate {
public:
  TableViewController(Escher::Responder * parent, Escher::SelectableTableView * tableView) :
      Escher::Responder(parent), m_seletableTableView(tableView) {}
  void didBecomeFirstResponder() override;

  Escher::SelectableTableView * selectableTableView() { return m_seletableTableView; }
  void setSeletableTableView(Escher::SelectableTableView * selectableTableView) {
    m_seletableTableView = selectableTableView;
  }

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::TextField * textField,
                                    Ion::Events::Event event) override;

protected:
  Escher::SelectableTableView * m_seletableTableView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_ABSTRACT_TABLE_VIEW_CONTROLLER_H */
