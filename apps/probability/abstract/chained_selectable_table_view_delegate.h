#ifndef APPS_PROBABILITY_ABSTRACT_CHAINED_SELECTABLE_TABLE_DELEGATE_H
#define APPS_PROBABILITY_ABSTRACT_CHAINED_SELECTABLE_TABLE_DELEGATE_H

#include <escher/selectable_table_view.h>

namespace Probability {

/* This class allows to chain SelectableTableViewDelegates in case there are several components
 * which need to be informed of a change in selection. */
class ChainedSelectableTableViewDelegate : public Escher::SelectableTableViewDelegate {
public:
  ChainedSelectableTableViewDelegate(
      Escher::SelectableTableViewDelegate * parentDelegate = nullptr) :
      m_parentDelegate(parentDelegate) {}

  void tableViewDidChangeSelection(Escher::SelectableTableView * t,
                                   int previousSelectedCellX,
                                   int previousSelectedCellY,
                                   bool withinTemporarySelection = false) {
    if (m_parentDelegate) {
      m_parentDelegate->tableViewDidChangeSelection(t,
                                                    previousSelectedCellX,
                                                    previousSelectedCellY,
                                                    withinTemporarySelection);
    }
  }
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t,
                                               int previousSelectedCellX,
                                               int previousSelectedCellY,
                                               bool withinTemporarySelection = false) {
    if (m_parentDelegate) {
      m_parentDelegate->tableViewDidChangeSelectionAndDidScroll(t,
                                                                previousSelectedCellX,
                                                                previousSelectedCellY,
                                                                withinTemporarySelection);
    }
  }

private:
  Escher::SelectableTableViewDelegate * m_parentDelegate;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_ABSTRACT_CHAINED_SELECTABLE_TABLE_DELEGATE_H */
