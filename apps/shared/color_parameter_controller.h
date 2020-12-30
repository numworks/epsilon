#ifndef SHARED_COLOR_PARAM_CONTROLLER_H
#define SHARED_COLOR_PARAM_CONTROLLER_H

#include <escher.h>
#include "function_store.h"
#include "color_cell.h"
#include <apps/i18n.h>

namespace Shared {

class ColorParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  ColorParameterController(Responder * parentResponder, I18n::Message title);

  View * view() override { return &m_selectableTableView; }
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;

  const char * title() override { return I18n::translate(m_title); }

  bool handleEvent(Ion::Events::Event event) override;

  TELEMETRY_ID("ColorParameter");

  void setRecord(Ion::Storage::Record record);

  int numberOfRows() const override { return Palette::numberOfDataColors(); }
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override { return Palette::numberOfDataColors(); } // FIXME Display issue
  void willDisplayCellForIndex(HighlightCell * cell, int index);
private:
  bool handleEnterOnRow(int rowIndex);
  FunctionStore * functionStore();
  ExpiringPointer<Function> function();
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
  I18n::Message m_title;
  MessageTableCellWithColor m_cells[Palette::numberOfDataColors()];
};

}

#endif
