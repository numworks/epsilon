#ifndef LOCALIZATION_CONTROLLER_H
#define LOCALIZATION_CONTROLLER_H

#include <escher/container.h>
#include <escher/expression_view.h>
#include <escher/message_table_cell.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/list_view_data_source.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>
#include <apps/i18n.h>
#include <algorithm>

namespace Shared {

class LocalizationController : public Escher::ViewController, public Escher::MemoizedListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  static int IndexOfCountry(I18n::Country country);
  static I18n::Country CountryAtIndex(int i);

  enum class Mode : uint8_t {
    Language,
    Country
  };

  LocalizationController(Escher::Responder * parentResponder, Mode mode);
  void resetSelection();
  Mode mode() const { return m_mode; }
  void setMode(Mode mode);

  virtual int indexOfCellToSelectOnReset() const;
  virtual bool shouldDisplayTitle() const = 0;
  bool shouldDisplayWarning() const { return mode() == Mode::Country; }

  Escher::View * view() override { return &m_contentView; }
  const char * title() override;
  void didBecomeFirstResponder() override { Escher::Container::activeApp()->setFirstResponder(selectableTableView()); }
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;

  int numberOfRows() const override { return (mode() == Mode::Country) ? I18n::NumberOfCountries : I18n::NumberOfLanguages; }
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override { return &m_cells[index]; }

  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

protected:
  class ContentView : public Escher::View {
  public:
    ContentView(LocalizationController * controller, Escher::SelectableTableViewDataSource * dataSource);

    Escher::SelectableTableView * selectableTableView() { return &m_selectableTableView; }
    void drawRect(KDContext * ctx, KDRect rect) const override { ctx->fillRect(bounds(), Escher::Palette::WallScreen); }
    void modeHasChanged();

  private:
    constexpr static int k_numberOfCountryWarningLines = 2;

    void layoutSubviews(bool force = false) override;
    KDCoordinate layoutTitleSubview(bool force, KDCoordinate verticalOrigin);
    KDCoordinate layoutWarningSubview(bool force, KDCoordinate verticalOrigin);
    KDCoordinate layoutTableSubview(bool force, KDCoordinate verticalOrigin);
    int numberOfSubviews() const override;
    Escher::View * subviewAtIndex(int i) override;

    LocalizationController * m_controller;
    Escher::SelectableTableView m_selectableTableView;
    Escher::MessageTextView m_countryTitleMessage;
    Escher::MessageTextView m_countryWarningLines[k_numberOfCountryWarningLines];
    Escher::SolidColorView m_borderView;
  };

  Escher::SelectableTableView * selectableTableView() { return m_contentView.selectableTableView(); }

  ContentView m_contentView;

private:
  void setVerticalMargins();
  constexpr static int k_numberOfCells = I18n::NumberOfLanguages > I18n::NumberOfCountries ? I18n::NumberOfLanguages : I18n::NumberOfCountries;
  Escher::MessageTableCell m_cells[k_numberOfCells];
  Mode m_mode;
};

}

#endif
