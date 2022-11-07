#ifndef APPS_MATH_VARIABLE_BOX_CONTROLLER_H
#define APPS_MATH_VARIABLE_BOX_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/nested_menu_controller.h>
#include <escher/expression_table_cell_with_expression.h>
#include <escher/message_table_cell_with_chevron.h>
#include <ion.h>

class MathVariableBoxController : public Escher::NestedMenuController {
public:
  MathVariableBoxController();

  // View Controller
  void viewDidDisappear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  //ListViewDataSource
  int numberOfRows() const override;
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  int typeAtIndex(int index) const override;

  // Menu
  enum class Page {
    RootMenu = 0,
    Expression = 1,
    Function = 2,
    List = 3,
    Matrix = 4,
    Sequence = 5,
    sizeOfEnum = 6 // use this value only to know the number of pages
  };
  void lockDeleteEvent(Page page) { m_lockPageDelete = page; }

private:
  constexpr static int k_maxNumberOfDisplayedRows = Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(Escher::TableCell::k_minimalLargeFontCellHeight, Escher::Metric::PopUpTopMargin + Escher::Metric::StackTitleHeight);
  constexpr static int k_numberOfMenuRows = static_cast<int>(Page::sizeOfEnum) - 1 /* RootMenu */ + 1 /* DefineVariable */;
  constexpr static KDCoordinate k_leafMargin = 20;
  constexpr static KDFont::Size k_subLabelFont = KDFont::Size::Small;
  constexpr static int k_defineVariableCellType = 2;
  Escher::ExpressionTableCellWithExpression * leafCellAtIndex(int index) override;
  int defineVariableCellIndex() const { return numberOfRows() - 1; }
  Escher::MessageTableCellWithChevron * nodeCellAtIndex(int index) override;
  I18n::Message subTitle() override;
  int numberOfElements(Page page) const;
  Page pageAtIndex(int index);
  void setPage(Page page);
  bool selectSubMenu(int selectedRow) override;
  bool returnToPreviousMenu() override;
  bool returnToRootMenu() override;
  bool selectLeaf(int selectedRow) override;
  I18n::Message nodeLabel(Page page);
  Poincare::Layout expressionLayoutForRecord(Ion::Storage::Record record, int index);
  const char * extension() const;
  Ion::Storage::Record recordAtIndex(int rowIndex);
  void resetVarBoxMemoization();
  // Return false if destruction is prevented by the app
  bool destroyRecordAtRowIndex(int rowIndex);
  Page m_currentPage;
  Page m_lockPageDelete;
  Escher::ExpressionTableCellWithExpression m_leafCells[k_maxNumberOfDisplayedRows];
  Escher::MessageTableCellWithChevron m_nodeCells[k_numberOfMenuRows];
  Escher::MessageTableCell m_defineVariableCell;
  // Layout memoization
  // TODO: make a helper doing the RingMemoizationOfConsecutiveObjets to factorize this code and ExpressionModelStore code
  int m_firstMemoizedLayoutIndex;
  Poincare::Layout m_layouts[k_maxNumberOfDisplayedRows];
};

#endif
