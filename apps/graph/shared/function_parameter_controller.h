#ifndef GRAPH_SHARED_FUNCTION_PARAMETER_CONTROLLER_H
#define GRAPH_SHARED_FUNCTION_PARAMETER_CONTROLLER_H

#include <apps/shared/column_parameter_controller.h>
#include <apps/shared/list_parameter_controller.h>
#include <escher/buffer_text_view.h>
#include <escher/chevron_view.h>
#include <escher/message_text_view.h>

#include "../graph/graph_controller.h"
#include "derivatives_parameter_controller.h"
#include "details_parameter_controller.h"
#include "domain_parameter_controller.h"

namespace Graph {

class ValuesController;

class FunctionParameterController : public Shared::ListParameterController,
                                    public Shared::ColumnParameters {
 public:
  FunctionParameterController(Escher::Responder* parentResponder);
  void setRecord(Ion::Storage::Record record) override;
  // MemoizedListViewDataSource
  Escher::HighlightCell* cell(int row) override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return k_numberOfRows; }
  const char* title() const override;
  TitlesDisplay titlesDisplay() const override {
    return TitlesDisplay::DisplayLastTwoTitles;
  }
  void setParameterDelegate(ParameterDelegate* parameterDelegate) {
    m_parameterDelegate = parameterDelegate;
  }

 private:
  // Shared cells + m_detailsCell + m_functionDomainCell + m_derivativesCell
  static constexpr int k_numberOfRows =
      Shared::ListParameterController::k_numberOfSharedCells + 3;
  bool displayDetails() const {
    assert(!Poincare::Preferences::SharedPreferences()
                ->examMode()
                .forbidGraphDetails() ||
           m_detailsParameterController.detailsNumberOfSections() == 0);
    return m_detailsParameterController.detailsNumberOfSections() > 0;
  }
  bool displayDomain() const {
    return m_domainParameterController.isVisible() > 0;
  }
  Shared::ExpiringPointer<Shared::ContinuousFunction> function();

  // ColumnParameters
  void initializeColumnParameters() override;
  Shared::ClearColumnHelper* clearColumnHelper() override;

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_detailsCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::OneLineBufferTextView<>,
                   Escher::ChevronView>
      m_functionDomainCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_derivativesCell;
  DetailsParameterController m_detailsParameterController;
  DomainParameterController m_domainParameterController;
  DerivativesParameterController m_derivativesParameterController;
  ParameterDelegate* m_parameterDelegate;
};

}  // namespace Graph

#endif
