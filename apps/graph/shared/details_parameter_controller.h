#ifndef GRAPH_SHARED_DETAILS_PARAMETER_CONTROLLER_H
#define GRAPH_SHARED_DETAILS_PARAMETER_CONTROLLER_H

#include <apps/shared/continuous_function.h>
#include <apps/shared/expiring_pointer.h>
#include <escher/buffer_text_view.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <ion/storage/file_system.h>
#include <poincare/conic.h>

namespace Graph {

using DetailCell =
    Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                     Escher::BufferTextView>;

class DetailsParameterController : public Escher::SelectableListViewController<
                                       Escher::MemoizedListViewDataSource>,
                                   public Escher::SelectableListViewDelegate {
 public:
  DetailsParameterController(Escher::Responder* parentResponder);

  bool handleEvent(Ion::Events::Event event) override;

  // ViewController
  const char* title() override;
  TitlesDisplay titlesDisplay() override {
    return TitlesDisplay::DisplayLastThreeTitles;
  }
  void viewWillAppear() override;
  TELEMETRY_ID("TypeParameter");

  // MemoizedListViewDataSource
  int numberOfRows() const override { return 1 + detailsNumberOfSections(); }
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  DetailCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return k_numberOfDataPoints; }
  int typeAtIndex(int index) const override { return 0; }

  // SelectableListViewDelegate
  bool canStoreContentOfCell(Escher::SelectableListView* t,
                             int row) const override {
    return row != k_indexOfCurveTypeRow;
  }

  // Number of sections to display in the ContinuousFunction's detail menu
  int detailsNumberOfSections() const;
  void setRecord(Ion::Storage::Record record);

 private:
  constexpr static int k_indexOfCurveTypeRow = 0;
  constexpr static size_t k_lineDetailsSections = 3;
  constexpr static size_t k_circleDetailsSections = 3;
  constexpr static size_t k_ellipseDetailsSections = 6;
  constexpr static size_t k_parabolaDetailsSections = 3;
  constexpr static size_t k_hyperbolaDetailsSections = 6;
  constexpr static int k_numberOfDataPoints = 7;  // max + 1 for plot type
  // Return record's Continuous Function pointer
  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;
  bool functionIsNonVerticalLine() const {
    return function()->properties().isLine() &&
           !function()->properties().isAlongY();
  }
  // Title of given section in ContinuousFunction's detail menu
  I18n::Message detailsTitle(int i) const;
  // Description of given section in ContinuousFunction's detail menu
  I18n::Message detailsDescription(int i) const;
  // Value of given section in ContinuousFunction's detail menu
  double detailsValue(int i) const {
    assert(i < detailsNumberOfSections());
    return m_detailValues[i];
  }
  // Set the detail values for a line
  void setLineDetailsValues(double slope, double intercept);
  // Set the detail values for a conic
  void setConicDetailsValues(Poincare::Conic* conic);

  Escher::StackViewController* stackController() const;
  DetailCell m_cells[k_numberOfDataPoints];
  double m_detailValues[k_numberOfDataPoints - 1];
  Ion::Storage::Record m_record;
};

}  // namespace Graph

#endif
