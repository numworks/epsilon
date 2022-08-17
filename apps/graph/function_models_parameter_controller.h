#ifndef GRAPH_FUNCTION_MODELS_PARAMETER_CONTROLLER_H
#define GRAPH_FUNCTION_MODELS_PARAMETER_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/expression_table_cell_with_message.h>
#include <escher/message_table_cell.h>
#include <escher/stack_view_controller.h>
#include <apps/i18n.h>

namespace Graph {

class ListController;

class FunctionModelsParameterController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource> {
public:
  FunctionModelsParameterController(Escher::Responder * parentResponder, void * functionStore, ListController * listController);
  const char * title() override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtIndex(int index) override { return index == k_indexOfEmptyModel ? k_emptyModelCellType : k_modelCellType; }
  int defaultName(char buffer[], size_t bufferSize) const;
private:
  constexpr static int k_emptyModelCellType = 0;
  constexpr static int k_modelCellType = 1;

  constexpr static int k_indexOfEmptyModel = 0;
  constexpr static int k_indexOfCartesianModel = k_indexOfEmptyModel + 1;
  constexpr static int k_indexOfImplicitModel = k_indexOfCartesianModel + 1;
  constexpr static int k_indexOfInequationModel = k_indexOfImplicitModel + 1;
  constexpr static int k_indexOfInverseModel = k_indexOfInequationModel + 1;
  constexpr static int k_indexOfConicModel = k_indexOfInverseModel + 1;
  constexpr static int k_indexOfParametricModel = k_indexOfConicModel + 1;
  constexpr static int k_indexOfPolarModel = k_indexOfParametricModel + 1;
  static_assert(k_indexOfEmptyModel == 0, "Empty model must be first.");

  constexpr static int k_numberOfModels = k_indexOfPolarModel + 1;
  constexpr static const char * k_models[k_numberOfModels] = {
    "", "f(x)=x", "x+y+1=0", "x+y≤0", "x=cos(y)", "x^2+y^2+x*y+x+y=0", "f(t)=[[cos(t)][sin(t)]]", "f(θ)=cos(θ)"
  };
  constexpr static const char * k_implicitModelWhenForbidden = "y=x-1";
  constexpr static const char * k_inequationModelWhenForbidden = "y≤x";
  constexpr static size_t k_maxSizeOfNamedModel = 26;
  // Expression cells
  constexpr static int k_numberOfExpressionCells = k_numberOfModels-1;
  constexpr static I18n::Message k_modelDescriptions[k_numberOfExpressionCells] = {
    I18n::Message::CartesianNamedTemplate, I18n::Message::LineType, I18n::Message::InequationType, I18n::Message::InverseType, I18n::Message::ConicNamedTemplate, I18n::Message::ParametricType, I18n::Message::PolarType
  };
  Escher::StackViewController * stackController() const;
  // Some models may be hidden. return the model index from a visible row index
  int getModelIndex(int row) const;
  // Some models may become forbidden and have an alternate form.
  const char * modelAtIndex(int index) const;
  Escher::MessageTableCell m_emptyModelCell;
  Escher::ExpressionTableCellWithMessage m_modelCells[k_numberOfExpressionCells];
  Poincare::Layout m_layouts[k_numberOfExpressionCells];
  void * m_functionStore;
  ListController * m_listController;
};

}

#endif
