#ifndef GRAPH_LIST_DETAILS_PARAMATER_CONTROLLER_H
#define GRAPH_LIST_DETAILS_PARAMATER_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/message_table_cell_with_message_with_buffer.h>
#include <escher/stack_view_controller.h>
#include <ion/storage/container.h>
#include "../../shared/continuous_function.h"
#include <poincare/conic.h>

namespace Graph {

class DetailsParameterController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource> {
public:
  DetailsParameterController(Escher::Responder * parentResponder);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  // ViewController
  const char * title() override;
  void viewWillAppear() override;
  TELEMETRY_ID("TypeParameter");

  // MemoizedListViewDataSource
  int numberOfRows() const override { return 1 + detailsNumberOfSections(); }
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  Escher::MessageTableCellWithMessageWithBuffer * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return k_numberOfDataPoints; }
  int typeAtIndex(int index) override { return 0; }
  // Number of sections to display in the ContinuousFunction's detail menu
  int detailsNumberOfSections() const;
  void setRecord(Ion::Storage::Record record);
private:
  static constexpr size_t k_lineDetailsSections = 3;
  static constexpr size_t k_circleDetailsSections = 3;
  static constexpr size_t k_ellipseDetailsSections = 6;
  static constexpr size_t k_parabolaDetailsSections = 3;
  static constexpr size_t k_hyperbolaDetailsSections = 6;
  static constexpr int k_numberOfDataPoints = 7; // max + 1 for plot type
  // Return record's Continuous Function pointer
  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;
  // Title of given section in ContinuousFunction's detail menu
  I18n::Message detailsTitle(int i) const;
  // Description of given section in ContinuousFunction's detail menu
  I18n::Message detailsDescription(int i) const;
  // Value of given section in ContinuousFunction's detail menu
  double detailsValue(int i) const { assert(i < detailsNumberOfSections()); return m_detailValues[i]; }
  // Set the detail values for a line
  void setLineDetailsValues(double slope, double intercept);
  // Set the detail values for a conic
  void setConicDetailsValues(Poincare::Conic conic);

  Escher::StackViewController * stackController() const;
  Escher::MessageTableCellWithMessageWithBuffer m_cells[k_numberOfDataPoints];
  double m_detailValues[k_numberOfDataPoints-1];
  Ion::Storage::Record m_record;
};

}

#endif
