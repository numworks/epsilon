#include "calculation_controller.h"
#include "../constant.h"
#include "../shared/poincare_helpers.h"
#include "app.h"
#include "calculation/discrete_calculation.h"
#include "calculation/left_integral_calculation.h"
#include "calculation/right_integral_calculation.h"
#include "calculation/finite_integral_calculation.h"
#include "images/calcul1_icon.h"
#include "images/calcul2_icon.h"
#include "images/calcul3_icon.h"
#include "images/calcul4_icon.h"
#include "images/focused_calcul1_icon.h"
#include "images/focused_calcul2_icon.h"
#include "images/focused_calcul3_icon.h"
#include "images/focused_calcul4_icon.h"
#include <assert.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Probability {

static inline int minInt(int x, int y) { return x < y ? x : y; }

CalculationController::ContentView::ContentView(SelectableTableView * selectableTableView, Law * law, Calculation * calculation) :
  m_titleView(KDFont::SmallFont, I18n::Message::ComputeProbability, 0.5f, 0.5f, Palette::GreyDark, Palette::WallScreen),
  m_selectableTableView(selectableTableView),
  m_lawCurveView(law, calculation)
{
}

int CalculationController::ContentView::numberOfSubviews() const {
  return 3;
}

View * CalculationController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 3);
  if (index == 0) {
    return &m_titleView;
  }
  if (index == 1) {
    return m_selectableTableView;
  }
  return &m_lawCurveView;
}

void CalculationController::ContentView::layoutSubviews() {
  KDCoordinate titleHeight = KDFont::SmallFont->glyphSize().height()+k_titleHeightMargin;
  m_titleView.setFrame(KDRect(0, 0, bounds().width(), titleHeight));
  KDCoordinate calculationHeight = ResponderImageCell::k_oneCellHeight+2*k_tableMargin;
  m_selectableTableView->setFrame(KDRect(0,  titleHeight, bounds().width(), calculationHeight));
  m_lawCurveView.setFrame(KDRect(0,  titleHeight+calculationHeight, bounds().width(), bounds().height() - calculationHeight - titleHeight));
}

CalculationController::CalculationController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Law * law, Calculation * calculation) :
  ViewController(parentResponder),
  m_contentView(&m_selectableTableView, law, calculation),
  m_selectableTableView(this),
  m_imageCell(&m_selectableTableView, law, calculation, this),
  m_draftTextBuffer{},
  m_calculation(calculation),
  m_law(law)
{
  assert(law != nullptr);
  assert(calculation != nullptr);
  m_selectableTableView.setMargins(k_tableMargin);
  m_selectableTableView.setVerticalCellOverlap(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
  m_selectableTableView.setBackgroundColor(KDColorWhite);


  for (int i = 0; i < k_numberOfCalculationCells; i++) {
    m_calculationCells[i].editableTextCell()->setParentResponder(&m_selectableTableView);
    m_calculationCells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, this);
    m_calculationCells[i].editableTextCell()->textField()->setDraftTextBuffer(m_draftTextBuffer);
  }
}

void CalculationController::didEnterResponderChain(Responder * previousResponder) {
  app()->snapshot()->setActivePage(App::Snapshot::Page::Calculations);
  updateTitle();
  reloadLawCurveView();
  m_selectableTableView.reloadData();
}

void CalculationController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_selectableTableView);
}

View * CalculationController::view() {
  return &m_contentView;
}

const char * CalculationController::title() {
  return m_titleBuffer;
}

void CalculationController::viewWillAppear() {
  ViewController::viewWillAppear();
  selectCellAtLocation(1, 0);
}

void CalculationController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
  ViewController::viewDidDisappear();
}

int CalculationController::numberOfRows() {
  return 1;
}

int CalculationController::numberOfColumns() {
  return m_calculation->numberOfParameters()+1;
}

/* WARNING: we set one type per cell to be able to deduce the column width from
 * the cell minimalSizeForOptimalDisplay. Otherwise, we can not know which cell
 * to interrogate to get the column width and we neither can call
 * tableView->cellAtLocation as this function depends on the
 * numberOfDisplaybleRows which depends on the column width! */

KDCoordinate CalculationController::columnWidth(int i) {
  if (i == 0) {
    return m_imageCell.minimalSizeForOptimalDisplay().width();
  }
  // WARNING: that is possible only because we know which view cell corresponds to which cell
  return m_calculationCells[i-1].minimalSizeForOptimalDisplay().width();
}

KDCoordinate CalculationController::rowHeight(int j) {
  return ResponderImageCell::k_oneCellHeight;
}

KDCoordinate CalculationController::cumulatedHeightFromIndex(int j) {
  return rowHeight(0) * j;
}

int CalculationController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  KDCoordinate height = rowHeight(0);
  if (height == 0) {
    return 0;
  }
  return (offsetY - 1) / height;
}

HighlightCell * CalculationController::reusableCell(int index, int type) {
  assert(index == 0);
  switch(type) {
    case 0:
      return &m_imageCell;
    default:
      return &m_calculationCells[type-1];
  }
}

int CalculationController::reusableCellCount(int type) {
  return 1;
}

int CalculationController::typeAtLocation(int i, int j) {
  return i;
}

void CalculationController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  if (i == 0) {
    ResponderImageCell * myCell = static_cast<ResponderImageCell *>(cell);
    const Image *  images[CalculationTypeController::k_numberOfImages] = {ImageStore::Calcul1Icon, ImageStore::Calcul2Icon, ImageStore::Calcul3Icon, ImageStore::Calcul4Icon};
    const Image * focusedImages[CalculationTypeController::k_numberOfImages] = {ImageStore::FocusedCalcul1Icon, ImageStore::FocusedCalcul2Icon, ImageStore::FocusedCalcul3Icon, ImageStore::FocusedCalcul4Icon};
    myCell->setImage(images[(int)m_calculation->type()], focusedImages[(int)m_calculation->type()]);
  } else {
    CalculationCell * myCell = static_cast<CalculationCell *>(cell);
    myCell->messageTextView()->setMessage(m_calculation->legendForParameterAtIndex(i-1));
    bool calculationCellIsResponder = true;
    if ((m_law->type() != Law::Type::Normal && i == 3) || (m_calculation->type() == Calculation::Type::Discrete && i == 2)) {
      calculationCellIsResponder = false;
    }
    myCell->setResponder(calculationCellIsResponder);
    TextField * field = static_cast<CalculationCell *>(cell)->editableTextCell()->textField();
    if (field->isEditing()) {
      return;
    }
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    PrintFloat::convertFloatToText<double>(m_calculation->parameterAtIndex(i-1), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
    field->setText(buffer);
  }
}

bool CalculationController::textFieldDidHandleEvent(::TextField * textField, bool returnValue, bool textSizeDidChange) {
  if (returnValue && textSizeDidChange) {
    /* We do not reload the responder because the first responder might be the
     * toolbox (or the variable  box) and reloading the responder would corrupt
     * the first responder. */
    bool shouldUpdateFirstResponder = app()->firstResponder() == textField;
    m_selectableTableView.reloadData(shouldUpdateFirstResponder);
    // The textField frame might have increased which forces to reload the textField scroll
    textField->scrollToCursor();
  }
  return returnValue;
}

bool CalculationController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event)
       || (event == Ion::Events::Right
           && textField->cursorLocation() == textField->text() + textField->draftTextLength()
           && selectedColumn() < m_calculation->numberOfParameters())
       || (event == Ion::Events::Left
           && textField->cursorLocation() == textField->text());
}

bool CalculationController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  if (m_calculation->type() != Calculation::Type::FiniteIntegral && selectedColumn() == 2) {
    if (floatBody < 0.0) {
      floatBody = 0.0;
    }
    if (floatBody > 1.0) {
      floatBody = 1.0;
    }
  }
  if (!m_law->isContinuous() && (selectedColumn() == 1 || m_calculation->type() == Calculation::Type::FiniteIntegral)) {
    floatBody = std::round(floatBody);
  }
  m_calculation->setParameterAtIndex(floatBody, selectedColumn()-1);
  if (event == Ion::Events::Right || event == Ion::Events::Left) {
    m_selectableTableView.handleEvent(event);
  }
  reload();
  return true;
}

void CalculationController::reloadLawCurveView() {
  m_contentView.lawCurveView()->reload();
}

void CalculationController::reload() {
  m_selectableTableView.reloadData();
  reloadLawCurveView();
}

void CalculationController::setCalculationAccordingToIndex(int index, bool forceReinitialisation) {
  if ((int)m_calculation->type() == index && !forceReinitialisation) {
    return;
  }
  m_calculation->~Calculation();
  switch (index) {
    case 0:
      new(m_calculation) LeftIntegralCalculation();
      break;
    case 1:
      new(m_calculation) FiniteIntegralCalculation();
      break;
    case 2:
      new(m_calculation) RightIntegralCalculation();
      break;
    case 3:
      new(m_calculation) DiscreteCalculation();
      break;
    default:
     return;
  }
  m_calculation->setLaw(m_law);
}

TextFieldDelegateApp * CalculationController::textFieldDelegateApp() {
  return app();
}

void CalculationController::updateTitle() {
  int currentChar = 0;
  for (int index = 0; index < m_law->numberOfParameter(); index++) {
    if (currentChar >= k_titleBufferSize) {
      break;
    }
    /* strlcpy returns the size of src, not the size copied, but it is not a
     * problem here. */
    currentChar += strlcpy(m_titleBuffer+currentChar, I18n::translate(m_law->parameterNameAtIndex(index)), k_titleBufferSize - currentChar);
    if (currentChar >= k_titleBufferSize) {
      break;
    }
    currentChar += strlcpy(m_titleBuffer+currentChar, " = ", k_titleBufferSize - currentChar);
    if (currentChar >= k_titleBufferSize) {
      break;
    }
    constexpr size_t bufferSize = PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits);
    char buffer[bufferSize];
    PrintFloat::convertFloatToText<double>(m_law->parameterValueAtIndex(index), buffer, bufferSize, Constant::ShortNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
    currentChar += strlcpy(m_titleBuffer+currentChar, buffer, k_titleBufferSize - currentChar);
    if (currentChar >= k_titleBufferSize) {
      break;
    }
    currentChar += UTF8Decoder::CodePointToChars(' ', m_titleBuffer + currentChar, k_titleBufferSize - currentChar);
  }
  m_titleBuffer[minInt(currentChar, k_titleBufferSize) - 1] = 0;
}

}
