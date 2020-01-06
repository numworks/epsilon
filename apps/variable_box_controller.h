#ifndef APPS_VARIABLE_BOX_CONTROLLER_H
#define APPS_VARIABLE_BOX_CONTROLLER_H

#define MATRIX_VARIABLES 1

#include <escher.h>
#include "variable_box_empty_controller.h"
#include <apps/i18n.h>

class VariableBoxController : public NestedMenuController {
public:
  VariableBoxController();

  // View Controller
  void viewWillAppear() override;
  void viewDidDisappear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  //ListViewDataSource
  int numberOfRows() const override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override;

  // Menu
  enum class Page {
    RootMenu = 0,
    Expression = 1,
    Function = 2
  };
  void lockDeleteEvent(Page page) { m_lockPageDelete = page; }
private:
 constexpr static int k_maxNumberOfDisplayedRows = (Ion::Display::Height - Metric::TitleBarHeight - Metric::PopUpTopMargin - Metric::StackTitleHeight) / Metric::ToolboxRowHeight + 2; // (240 - 18 - 50 - 20) / 40  = 3.8; the 0.8 cell can be above and below so we add +2 to get 5
  constexpr static int k_numberOfMenuRows = 2;
  constexpr static KDCoordinate k_leafMargin = 20;
  ExpressionTableCellWithExpression * leafCellAtIndex(int index) override;
  MessageTableCellWithChevron * nodeCellAtIndex(int index) override;
  Page pageAtIndex(int index);
  void setPage(Page page);
  bool selectSubMenu(int selectedRow) override;
  bool returnToPreviousMenu() override;
  bool selectLeaf(int selectedRow) override;
  I18n::Message nodeLabelAtIndex(int index);
  Poincare::Layout expressionLayoutForRecord(Ion::Storage::Record record, int index);
  const char * extension() const;
  Ion::Storage::Record recordAtIndex(int rowIndex);
  bool displayEmptyController();
  bool isDisplayingEmptyController() { return StackViewController::depth() == 2; }
  void resetMemoization();
  void destroyRecordAtRowIndex(int rowIndex);
  Page m_currentPage;
  Page m_lockPageDelete;
  ExpressionTableCellWithExpression m_leafCells[k_maxNumberOfDisplayedRows];
  MessageTableCellWithChevron m_nodeCells[k_numberOfMenuRows];
  VariableBoxEmptyController m_emptyViewController;
  // Layout memoization
  // TODO: make a helper doing the RingMemoizationOfConsecutiveObjets to factorize this code and ExpressionModelStore code
  int m_firstMemoizedLayoutIndex;
  Poincare::Layout m_layouts[k_maxNumberOfDisplayedRows];
};

#endif
