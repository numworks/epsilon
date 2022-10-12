#ifndef APPS_MATH_VARIABLE_BOX_CONTROLLER_H
#define APPS_MATH_VARIABLE_BOX_CONTROLLER_H

#include "alternate_empty_nested_menu_controller.h"
#include "math_variable_box_empty_controller.h"
#include <apps/i18n.h>
#include <escher/expression_table_cell_with_expression.h>
#include <escher/message_table_cell_with_chevron_and_buffer.h>
#include <ion.h>

class MathVariableBoxController : public AlternateEmptyNestedMenuController {
public:
  MathVariableBoxController();

  // View Controller
  void viewWillAppear() override;
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
  constexpr static int k_maxNumberOfDisplayedRows = Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(Escher::TableCell::k_minimalLargeFontCellHeight, Escher::Metric::PopUpTopMargin + Escher::Metric::StackTitleHeight); // Remaining cell can be above and below so we add +2
  constexpr static int k_numberOfMenuRows = static_cast<int>(Page::sizeOfEnum) - 1 + 1;
  constexpr static KDCoordinate k_leafMargin = 20;
  constexpr static KDFont::Size k_subLabelFont = KDFont::Size::Small;
  constexpr static int k_defineVariableCellType = 2;
  constexpr static int k_defineVariableCellIndex = k_numberOfMenuRows - 1;
  Escher::ExpressionTableCellWithExpression * leafCellAtIndex(int index) override;
  Escher::MessageTableCellWithChevron * nodeCellAtIndex(int index) override;
  I18n::Message subTitle() override;
  int numberOfElements(Page page) const;
  Page pageAtIndex(int index);
  void setPage(Page page);
  bool selectSubMenu(int selectedRow) override;
  bool returnToPreviousMenu() override;
  bool returnToRootMenu() override;
  bool selectLeaf(int selectedRow) override;
  I18n::Message nodeLabelAtIndex(int index);
  Poincare::Layout expressionLayoutForRecord(Ion::Storage::Record record, int index);
  const char * extension() const;
  Ion::Storage::Record recordAtIndex(int rowIndex);
  Escher::ViewController * emptyViewController() override;
  void resetVarBoxMemoization();
  void destroyRecordAtRowIndex(int rowIndex);
  Page m_currentPage;
  Page m_lockPageDelete;
  Escher::ExpressionTableCellWithExpression m_leafCells[k_maxNumberOfDisplayedRows];
  Escher::MessageTableCellWithChevronAndBuffer m_nodeCells[k_numberOfMenuRows];
  Escher::MessageTableCell m_defineVariableCell;
  MathVariableBoxEmptyController m_emptyViewController;
  // Layout memoization
  // TODO: make a helper doing the RingMemoizationOfConsecutiveObjets to factorize this code and ExpressionModelStore code
  int m_firstMemoizedLayoutIndex;
  Poincare::Layout m_layouts[k_maxNumberOfDisplayedRows];
};

#endif
