#ifndef APPS_SHARED_FORMULA_TEMPLATE_MENU_CONTROLLER_H
#define APPS_SHARED_FORMULA_TEMPLATE_MENU_CONTROLLER_H

#include <escher/nested_menu_controller.h>
#include <apps/i18n.h>
#include <escher/message_table_cell.h>
#include <escher/expression_table_cell_with_message.h>
#include <escher/expression_table_cell_with_buffer.h>
#include <escher/selectable_list_view_controller.h>

namespace Shared {

class StoreController;

class FormulaTemplateMenuController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource> {
public:
  constexpr static int k_maxSizeOfTemplateText = 30;
  FormulaTemplateMenuController(Escher::Responder * parentResponder, StoreController * storeController);

  const char * title() override;
  void viewWillAppear() override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int i) override;
  void viewDidDisappear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override {
    return k_numberOfTemplates - (m_displayOtherAppCell ? 0 : 1);
  }
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtIndex(int index) override;

private:
  enum class Cell {
    EmptyTemplate = 0,
    Integers = 1,
    Random = 2,
    Logarithm = 3,
    OtherColumns = 4,
    OtherApp = 5,
    maxNumberOfRows = 6
  };
  constexpr static int k_numberOfTemplates = static_cast<int>(Cell::maxNumberOfRows);
  constexpr static int k_numberOfExpressionCellsWithMessage = 1 + (int)Cell::Logarithm - (int)Cell::Integers;
  constexpr static int k_numberOfExpressionCellsWithBuffer = 1 + (int)Cell::OtherApp - (int)Cell::OtherColumns;
  static_assert(k_numberOfTemplates == 1 + k_numberOfExpressionCellsWithMessage + k_numberOfExpressionCellsWithBuffer, "Wrong number of cells in FormulaTemplateMenuController");
  enum class CellType {
    EmptyTemplate = 0,
    TemplateWithMessage = 1,
    TemplateWithBuffer = 2
  };
  constexpr static const char * k_templates[k_numberOfExpressionCellsWithMessage] = {
    "sequence(k,k,10)",
    "sequence(random(),k,10)",
    "sequence(ln(k),k,10)"
  };
  // TODO : Replace with translations
  constexpr static I18n::Message k_subLabelMessages[k_numberOfExpressionCellsWithMessage + k_numberOfExpressionCellsWithBuffer] = {
    I18n::Message::FormulaTemplateInt,
    I18n::Message::FormulaTemplateRand,
    I18n::Message::FormulaTemplateLog,
    I18n::Message::FormulaTemplateSum,
    I18n::Message::FormulaTemplateList,
  };

  int relativeCellIndex(int index, CellType type);
  bool shouldDisplayOtherAppCell();
  Poincare::Expression templateExpressionForCell(Cell cell);
  void computeUninitializedLayouts();
  void fillSubLabelBuffer(Escher::ExpressionTableCellWithBuffer * cell, int index);
  void fillSumColumnNames(char * buffers[]);
  void fillOtherAppColumnName(char * buffer);

  Escher::MessageTableCell m_emptyTemplateCell;
  Escher::ExpressionTableCellWithMessage m_templatesWithMessage[k_numberOfExpressionCellsWithMessage];
  Escher::ExpressionTableCellWithBuffer m_templatesWithBuffer[k_numberOfExpressionCellsWithBuffer];
  Poincare::Layout m_layouts[k_numberOfTemplates - 1];
  StoreController * m_storeController;
  bool m_displayOtherAppCell;
};

}
#endif
