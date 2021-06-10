#ifndef GRAPH_FUNCTION_MODELS_PARAMETER_CONTROLLER_H
#define GRAPH_FUNCTION_MODELS_PARAMETER_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/expression_table_cell_with_message.h>
#include <escher/message_table_cell.h>
#include <escher/stack_view_controller.h>
#include <apps/i18n.h>

namespace Graph {

class ListController;

class FunctionModelsParameterController : public Escher::SelectableListViewController {
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
  int typeAtIndex(int index) override { return index == 0 ? k_emptyModelCellType : k_modelCellType; }
private:
  constexpr static int k_emptyModelCellType = 0;
  constexpr static int k_modelCellType = 1;
  constexpr static int k_numberOfModels = 7;
  static constexpr const char * k_models[k_numberOfModels] = {
    "", "f(x)=x", "x+y+1=0", "-2x+4+x^2+y^2-2x+4y+6=0", "x^2+y^2-2x+4y+6=0", "-2x+4+x^2+y^2-2x+4y+6=0", "-2x+4+x^2+y^2-2x+4y+6=0" // "x+yinferior<=0", "x^2+y^2-2x+4y+6=0", "f(t)=[[cos(t)],[sin(t)]]", "f(θ)=cos(θ)"
  };
  constexpr static int k_numberOfExpressionCells = k_numberOfModels-1;
  static constexpr I18n::Message k_modelDescriptions[k_numberOfExpressionCells] = {
    I18n::Message::CartesianType, I18n::Message::CurveType, I18n::Message::CurveType, I18n::Message::CurveType, I18n::Message::ParametricType, I18n::Message::PolarType
  };
  Escher::StackViewController * stackController() const;
  Escher::MessageTableCell m_emptyModelCell;
  Escher::ExpressionTableCellWithMessage m_modelCells[k_numberOfExpressionCells];
  Poincare::Layout m_layouts[k_numberOfExpressionCells];
  void * m_functionStore;
  ListController * m_listController;
};

}

#endif
