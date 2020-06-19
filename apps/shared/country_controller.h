#ifndef SHARED_COUNTRY_CONTROLLER_H
#define SHARED_COUNTRY_CONTROLLER_H

#include <escher.h>
#include <apps/i18n.h>

namespace Shared {

class CountryController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  static int IndexOfCountry(I18n::Country country);
  static I18n::Country CountryAtIndex(int i);

  CountryController(Responder * parentResponder, KDCoordinate verticalMargin);
  virtual void resetSelection();

  View * view() override { return &m_selectableTableView; }
  const char * title() override {return I18n::translate(I18n::Message::Country); }
  void didBecomeFirstResponder() override {Container::activeApp()->setFirstResponder(&m_selectableTableView); }
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;

  int numberOfRows() const override { return I18n::NumberOfCountries; }
  KDCoordinate cellHeight() override { return Metric::ParameterCellHeight; }
  HighlightCell * reusableCell(int index) override { return &m_cells[index]; }
  int reusableCellCount() const override { return I18n::NumberOfCountries; }

  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

protected:
  SelectableTableView m_selectableTableView;

private:
  MessageTableCell m_cells[I18n::NumberOfCountries];
  // TODO : Add variables for static text
};

}

#endif
