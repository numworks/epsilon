#ifndef SHARED_LANGUAGE_CONTROLLER_H
#define SHARED_LANGUAGE_CONTROLLER_H

#include <escher.h>
#include "../i18n.h"

namespace Shared {

class LanguageController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  LanguageController(Responder * parentResponder, KDCoordinate topMargin);
  void resetSelection();

  View * view() override;
  const char * title() override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;

  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;

  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  SelectableTableView m_selectableTableView;
  MessageTableCell m_cells[I18n::NumberOfLanguages];
};

}

#endif
