#ifndef APPS_VARIABLE_BOX_CONTROLLER_H
#define APPS_VARIABLE_BOX_CONTROLLER_H

#define MATRIX_VARIABLES 1

#include <escher.h>
#include "shared/global_context.h"
#include "variable_box_empty_controller.h"
#include "i18n.h"

class VariableBoxController : public NestedMenuController {
public:
  VariableBoxController();

  // View Controller
  void viewWillAppear() override;
  void viewDidDisappear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  //ListViewDataSource
  int numberOfRows() override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override;

  // Menu
private:
  enum class Page {
    RootMenu = 0,
    Expression = 1,
    Function = 2
  };
  // TODO: use the "(x)" define in CartesianFunctionSomething
  constexpr static int k_functionArgLength = 3;
  constexpr static const char * k_functionArg = "(x)";
  constexpr static int k_maxNumberOfDisplayedRows = 6; //240/Metric::ToolboxRowHeight
  constexpr static int k_numberOfMenuRows = 2;
  constexpr static KDCoordinate k_leafMargin = 20;
  ExpressionTableCellWithExpression * leafCellAtIndex(int index) override;
  MessageTableCellWithChevron * nodeCellAtIndex(int index) override;
  Page pageAtIndex(int index);
  bool selectSubMenu(int selectedRow) override;
  bool returnToPreviousMenu() override;
  bool selectLeaf(int selectedRow) override;
  I18n::Message nodeLabelAtIndex(int index);
  Poincare::Layout expressionLayoutForRecord(Ion::Storage::Record record);
  const char * extension() const;
  Ion::Storage::Record recordAtIndex(int rowIndex);
  bool displayEmptyController();
  bool isDisplayingEmptyController() { return StackViewController::depth() == 2; }
  Page m_currentPage;
  ExpressionTableCellWithExpression m_leafCells[k_maxNumberOfDisplayedRows];
  MessageTableCellWithChevron m_nodeCells[k_numberOfMenuRows];
  VariableBoxEmptyController m_emptyViewController;
  // Layout memoization
  //Poincare::Layout m_layouts[k_maxNumberOfDisplayedRows];
};

#endif
