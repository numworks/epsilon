#ifndef REGRESSION_REGRESSION_CONTROLLER_H
#define REGRESSION_REGRESSION_CONTROLLER_H

#include <apps/global_preferences.h>
#include <apps/i18n.h>
#include <escher/expression_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>

#include "../model/model.h"
#include "../store.h"

namespace Regression {

class RegressionController : public Escher::SelectableListViewController<
                                 Escher::MemoizedListViewDataSource> {
 public:
  RegressionController(Escher::Responder* parentResponder, Store* store);
  void setSeries(int series) { m_series = series; }
  void setDisplayedFromDataTab(bool status) { m_displayedFromDataTab = status; }
  // ViewController
  const char* title() override;
  ViewController::TitlesDisplay titlesDisplay() override;
  TELEMETRY_ID("Regression");

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // MemoizedListViewDataSource
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return k_numberOfCells; }
  int numberOfRows() const override {
    return GlobalPreferences::sharedGlobalPreferences->regressionAppVariant() ==
                   CountryPreferences::RegressionApp::Default
               ? k_defaultNumberOfRows
               : k_variantNumberOfRows;
  }
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

 private:
  // In all variants, Model::Type::None isn't made available.
  // Default - Hides LinearApbx
  constexpr static int k_defaultNumberOfRows = Model::k_numberOfModels - 2;
  constexpr static Model::Type DefaultModelTypeAtIndex(int index) {
    return static_cast<Model::Type>(index + 1);
  }
  constexpr static int DefaultIndexOfModelType(Model::Type type) {
    return static_cast<int>(type) - 1;
  }
  // Variant - Reorder displayed models
  constexpr static int k_variantNumberOfRows = Model::k_numberOfModels - 1;
  constexpr static Model::Type VariantModel[k_variantNumberOfRows] = {
      Model::Type::LinearAxpb,      Model::Type::LinearApbx,
      Model::Type::Proportional,    Model::Type::Quadratic,
      Model::Type::Cubic,           Model::Type::Quartic,
      Model::Type::Power,           Model::Type::ExponentialAbx,
      Model::Type::ExponentialAebx, Model::Type::Logarithmic,
      Model::Type::Logistic,        Model::Type::Trigonometric,
      Model::Type::Median,
  };
  constexpr static int VariantModelIndex[Model::k_numberOfModels] = {
      -1,  // Model::Type::None
      0,   // Model::Type::LinearAxpb
      2,   // Model::Type::Proportional
      3,   // Model::Type::Quadratic
      4,   // Model::Type::Cubic
      5,   // Model::Type::Quartic
      9,   // Model::Type::Logarithmic
      8,   // Model::Type::ExponentialAebx
      7,   // Model::Type::ExponentialAbx
      6,   // Model::Type::Power
      11,  // Model::Type::Trigonometric
      10,  // Model::Type::Logistic
      12,  // Model::Type::Median
      1,   // Model::Type::LinearApbx
  };
  constexpr static Model::Type VariantModelTypeAtIndex(int index) {
    return VariantModel[index];
  }
  constexpr static int VariantIndexOfModelType(Model::Type type) {
    return VariantModelIndex[static_cast<int>(type)];
  }

  static Model::Type ModelTypeAtIndex(int index) {
    return GlobalPreferences::sharedGlobalPreferences->regressionAppVariant() ==
                   CountryPreferences::RegressionApp::Default
               ? DefaultModelTypeAtIndex(index)
               : VariantModelTypeAtIndex(index);
  }
  static int IndexOfModelType(Model::Type type) {
    return GlobalPreferences::sharedGlobalPreferences->regressionAppVariant() ==
                   CountryPreferences::RegressionApp::Default
               ? DefaultIndexOfModelType(type)
               : VariantIndexOfModelType(type);
  }
  // Remaining cell can be above and below so we add +2
  constexpr static int k_numberOfCells =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::AbstractMenuCell::k_minimalLargeFontCellHeight,
          Escher::Metric::TabHeight + 2 * Escher::Metric::StackTitleHeight);

  // Display X?/Y? only when no other displayed title already names the series.
  bool displaySeriesNameAsTitle() const {
    return !m_displayedFromDataTab &&
           m_store->seriesSatisfy(m_series, Store::HasCoefficients);
  }

  Escher::MenuCell<Escher::MessageTextView, Escher::ExpressionView>
      m_regressionCells[k_numberOfCells];
  Store* m_store;
  int m_series;
  bool m_displayedFromDataTab;
};

}  // namespace Regression

#endif
