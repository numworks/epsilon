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

  View * view() override { return &m_contentView; }
  const char * title() override { return I18n::translate(I18n::Message::Country); }
  void didBecomeFirstResponder() override { Container::activeApp()->setFirstResponder(selectableTableView()); }
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;

  int numberOfRows() const override { return I18n::NumberOfCountries; }
  KDCoordinate cellHeight() override { return Metric::ParameterCellHeight; }
  HighlightCell * reusableCell(int index) override { return &m_cells[index]; }
  int reusableCellCount() const override { return I18n::NumberOfCountries; }

  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

protected:
  class ContentView : public View {
  public:
    ContentView(CountryController * controller, SelectableTableViewDataSource * dataSource);
    SelectableTableView * selectableTableView() { return &m_selectableTableView; }
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void shouldDisplayTitle(bool flag) { m_displayTitle = flag; }
  protected:
    void layoutSubviews(bool force = false) override;
    KDCoordinate layoutTitleSubview(bool force, KDCoordinate verticalOrigin);
    KDCoordinate layoutSubtitleSubview(bool force, KDCoordinate verticalOrigin);
    KDCoordinate layoutTableSubview(bool force, KDCoordinate verticalOrigin);
  private:
    constexpr static int k_numberOfTextLines = 2;
    int numberOfSubviews() const override { return 1 + 1 + k_numberOfTextLines; }
    View * subviewAtIndex(int i) override;
    SelectableTableView m_selectableTableView;
    MessageTextView m_titleMessage;
    MessageTextView m_messageLines[k_numberOfTextLines];
    bool m_displayTitle;
  };

  SelectableTableView * selectableTableView() { return m_contentView.selectableTableView(); }
  ContentView m_contentView;

private:
  MessageTableCell m_cells[I18n::NumberOfCountries];
};

}

#endif
