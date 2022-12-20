#ifndef GRAPH_FUNCTION_MODELS_PARAMETER_CONTROLLER_H
#define GRAPH_FUNCTION_MODELS_PARAMETER_CONTROLLER_H

#include <apps/i18n.h>
#include <apps/shared/continuous_function.h>
#include <escher/explicit_selectable_list_view_controller.h>
#include <escher/expression_table_cell_with_message.h>
#include <escher/message_table_cell.h>
#include <escher/stack_view_controller.h>

namespace Graph {

class ListController;

class FunctionModelsParameterController : public Escher::ExplicitSelectableListViewController {
public:
  FunctionModelsParameterController(Escher::Responder * parentResponder, void * functionStore, ListController * listController);
  const char * title() override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event & event) override;
  int numberOfRows() const override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  Escher::HighlightCell * cell(int index) override;
  static int DefaultName(char buffer[], size_t bufferSize);
  /* Tell if the country prefers y=x or f(x)=x */
  static bool EquationsPrefered() { return Models()[0] == Model::Equation; }
private:
  enum class Model : uint8_t {
    Empty = 0,
    Equation,
    Function,
    Line,
    LineVariant,
    Inequality,
    Inverse,
    Conic,
    Parametric,
    Polar,
    Piecewise,
    NumberOfModels
  };

  constexpr static Model layoutDefault[] = {
    Model::Function,
    Model::Line,
    Model::Inequality,
    Model::Inverse,
    Model::Piecewise,
    Model::Conic,
    Model::Parametric,
    Model::Polar,
  };

  constexpr static Model layoutVariant1[] = { // US
    Model::Equation,
    Model::Function,
    Model::Inequality,
    Model::Inverse,
    Model::Piecewise,
    Model::Conic,
    Model::Parametric,
    Model::Polar
  };

  constexpr static Model layoutVariant2[] = { // PT
    Model::Function,
    Model::Piecewise,
    Model::LineVariant,
    Model::Inequality,
    Model::Conic,
    Model::Inverse,
    Model::Polar,
    Model::Parametric,
  };

  constexpr static int k_numberOfExpressionModels = static_cast<int>(Model::NumberOfModels)-1;

  static_assert(sizeof(layoutDefault) == sizeof(layoutVariant1) && sizeof(layoutDefault) == sizeof(layoutVariant2), "Template layouts are assumed to be the same length in all countries");
  constexpr static int k_numberOfExpressionCells = static_cast<int>(sizeof(layoutDefault) / sizeof(Model));

  constexpr static const char * k_models[static_cast<int>(Model::NumberOfModels)] = {
    "", "y=x", "f(x)=x", "x+y+1=0", "x+y=1", "x+y≤0", "x=√(y)", "x^2+y^2+x*y+x+y=0", "f(t)=[[cos(t)][sin(t)]]", "r=cos(θ)", "f(x)=piecewise(-x,x<0,x,x≥0)"
  };
  constexpr static const char * k_lineModelWhenForbidden = "y=x-1";
  constexpr static const char * k_inequationModelWhenForbidden = "y≤x";
  // Piecewise is the longest named model
  constexpr static size_t k_maxSizeOfNamedModel = Poincare::Helpers::StringLength(k_models[static_cast<int>(Model::Piecewise)]) - 1 + Shared::ContinuousFunction::k_maxDefaultNameSize;
  // Expression cells
  constexpr static I18n::Message k_modelDescriptions[k_numberOfExpressionModels] = {
    I18n::Message::LineEquationTitle, I18n::Message::CartesianNamedTemplate, I18n::Message::LineType, I18n::Message::LineType, I18n::Message::InequalityType, I18n::Message::InverseFunction,  I18n::Message::ConicNamedTemplate, I18n::Message::ParametricEquationType, I18n::Message::PolarEquationType, I18n::Message::PiecewiseFunction
  };
  // The models list depends on the current country
  static const Model * Models();
  // Some models may be hidden.
  static bool ModelIsAllowed(Model model);
  // Some models may become forbidden and have an alternate form.
  static const char * ModelString(Model model);
  static const char * ModelWithDefaultName(Model model, char buffer[], size_t bufferSize);
  Escher::StackViewController * stackController() const;
  Escher::MessageTableCell m_emptyModelCell;
  Escher::ExpressionTableCellWithMessage m_modelCells[k_numberOfExpressionCells];
  Poincare::Layout m_layouts[k_numberOfExpressionCells];
  void * m_functionStore;
  ListController * m_listController;
};

}

#endif
