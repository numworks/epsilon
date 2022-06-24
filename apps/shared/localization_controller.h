#ifndef LOCALIZATION_CONTROLLER_H
#define LOCALIZATION_CONTROLLER_H

#include <escher.h>
#include <apps/i18n.h>
#include <algorithm>

namespace Shared {

class LocalizationController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  static int IndexOfCountry(I18n::Country country);
  static I18n::Country CountryAtIndex(int i);

  enum class Mode : uint8_t {
    Language,
    Country
  };

  LocalizationController(Responder * parentResponder, KDCoordinate verticalMargin, Mode mode);
  void resetSelection();
  Mode mode() const { return m_mode; }
  void setMode(Mode mode);

  virtual int indexOfCellToSelectOnReset() const;
  virtual bool shouldDisplayTitle() const = 0;
  bool shouldDisplayWarning() const { return mode() == Mode::Country; }

  View * view() override { return &m_contentView; }
  const char * title() override;
  void didBecomeFirstResponder() override { Container::activeApp()->setFirstResponder(selectableTableView()); }
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;

  int numberOfRows() const override { return (mode() == Mode::Country) ? I18n::NumberOfCountries : I18n::NumberOfLanguages; }
  KDCoordinate cellHeight() override { return Metric::ParameterCellHeight; }
  HighlightCell * reusableCell(int index) override { return &m_cells[index]; }
  int reusableCellCount() const override { return (mode() == Mode::Country) ? I18n::NumberOfCountries : I18n::NumberOfLanguages; }

  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

protected:
  class ContentView : public View {
  public:
    ContentView(LocalizationController * controller, SelectableTableViewDataSource * dataSource);

    SelectableTableView * selectableTableView() { return &m_selectableTableView; }
    void drawRect(KDContext * ctx, KDRect rect) const override { ctx->fillRect(bounds(), Palette::BackgroundApps); }
    void modeHasChanged();

  private:
    constexpr static int k_numberOfCountryWarningLines = 2;

    void layoutSubviews(bool force = false) override;
    KDCoordinate layoutTitleSubview(bool force, KDCoordinate verticalOrigin);
    KDCoordinate layoutWarningSubview(bool force, KDCoordinate verticalOrigin);
    KDCoordinate layoutTableSubview(bool force, KDCoordinate verticalOrigin);
    int numberOfSubviews() const override;
    View * subviewAtIndex(int i) override;

    LocalizationController * m_controller;
    SelectableTableView m_selectableTableView;
    MessageTextView m_countryTitleMessage;
    MessageTextView m_countryWarningLines[k_numberOfCountryWarningLines];
    SolidColorView m_borderView;
  };

  SelectableTableView * selectableTableView() { return m_contentView.selectableTableView(); }

  ContentView m_contentView;

private:
  static constexpr int k_numberOfCells = I18n::NumberOfLanguages > I18n::NumberOfCountries ? I18n::NumberOfLanguages : I18n::NumberOfCountries;
  MessageTableCell<> m_cells[k_numberOfCells];
  Mode m_mode;
};

}

#endif
