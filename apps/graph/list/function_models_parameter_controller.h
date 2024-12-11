#ifndef GRAPH_FUNCTION_MODELS_PARAMETER_CONTROLLER_H
#define GRAPH_FUNCTION_MODELS_PARAMETER_CONTROLLER_H

#include <apps/i18n.h>
#include <apps/shared/continuous_function.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/scrollable_layout_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <omg/string.h>

#include <array>

namespace Graph {

class ListController;

class FunctionModelsParameterController
    : public Escher::ExplicitSelectableListViewController {
 public:
  FunctionModelsParameterController(Escher::Responder* parentResponder,
                                    ListController* listController);
  const char* title() const override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  Escher::HighlightCell* cell(int row) override;
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
    Point,
    ListOfPoints,
    NumberOfModels
  };

  constexpr static Model layoutDefault[] = {
      Model::Function,   Model::Line,  Model::Inequality,   Model::Inverse,
      Model::Piecewise,  Model::Point, Model::ListOfPoints, Model::Conic,
      Model::Parametric, Model::Polar,
  };

  constexpr static Model layoutVariant1[] = {
      // US
      Model::Equation,     Model::Function, Model::Inequality, Model::Point,
      Model::ListOfPoints, Model::Inverse,  Model::Piecewise,  Model::Conic,
      Model::Parametric,   Model::Polar,
  };

  constexpr static Model layoutVariant2[] = {
      // PT
      Model::Function,     Model::Piecewise,  Model::LineVariant,
      Model::Inequality,   Model::Conic,      Model::Inverse,
      Model::Polar,        Model::Parametric, Model::Point,
      Model::ListOfPoints,
  };

  constexpr static int k_numberOfExpressionModels =
      static_cast<int>(Model::NumberOfModels) - 1;

  static_assert(
      sizeof(layoutDefault) == sizeof(layoutVariant1) &&
          sizeof(layoutDefault) == sizeof(layoutVariant2),
      "Template layouts are assumed to be the same length in all countries");
  constexpr static int k_numberOfExpressionCells = std::size(layoutDefault);

  constexpr static const char*
      k_models[static_cast<int>(Model::NumberOfModels)] = {
          "",
          "y=x",
          "f(x)=x",
          "x+y+1=0",
          "x+y=1",
          "x+y≤0",
          "x=√(y)",
          "x^2+y^2+x*y+x+y=0",
          "f(t)=(cos(t),sin(t))",
          "r1(θ)=cos(θ)",
          "f(x)=piecewise(-x,x<0,x,x≥0)",
          "(2,3)",
          "{(2,3),(3,5)}",
      };
  constexpr static const char* k_lineModelWhenForbidden = "y=x-1";
  constexpr static const char* k_inequationModelWhenForbidden = "y≤x";
  // Piecewise is the longest named model
  constexpr static size_t k_maxSizeOfNamedModel =
      OMG::StringLength(k_models[static_cast<int>(Model::Piecewise)]) - 1 +
      Shared::ContinuousFunction::k_maxDefaultNameSize;
  // Expression cells
  constexpr static I18n::Message
      k_modelDescriptions[k_numberOfExpressionModels] = {
          I18n::Message::Equation,
          I18n::Message::Function,
          I18n::Message::LineType,
          I18n::Message::LineType,
          I18n::Message::InequalityType,
          I18n::Message::InverseFunction,
          I18n::Message::ConicNamedTemplate,
          I18n::Message::ParametricEquationType,
          I18n::Message::PolarEquationType,
          I18n::Message::PiecewiseFunction,
          I18n::Message::PointType,
          I18n::Message::ListOfPointsType,
      };
  // The models list depends on the current country
  static const Model* Models();
  // Some models may be hidden.
  static bool ModelIsAllowed(Model model);
  // Some models may become forbidden and have an alternate form.
  static const char* ModelString(Model model);
  static const char* ModelWithDefaultName(Model model, char buffer[],
                                          size_t bufferSize);
  Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView>
      m_modelCells[k_numberOfExpressionCells];
  Poincare::Layout m_layouts[k_numberOfExpressionCells];
  ListController* m_listController;
};

}  // namespace Graph

#endif
