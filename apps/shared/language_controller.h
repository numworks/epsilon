#ifndef SHARED_LANGUAGE_CONTROLLER_H
#define SHARED_LANGUAGE_CONTROLLER_H

#include <escher.h>
#include <apps/i18n.h>

namespace Shared {

class LanguageController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  LanguageController(Responder * parentResponder, KDCoordinate verticalMargin);
  void resetSelection();

  View * view() override;
  const char * title() override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;

  int numberOfRows() const override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;

  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

protected:
  SelectableTableView m_selectableTableView;

private:
  MessageTableCell m_cells[I18n::NumberOfLanguages];
};

}

#endif
