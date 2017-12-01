#include "calculation_controller.h"
#include "../constant.h"
#include "../apps_container.h"
#include "app.h"
#include "calculation/discrete_calculation.h"
#include "calculation/left_integral_calculation.h"
#include "calculation/right_integral_calculation.h"
#include "calculation/finite_integral_calculation.h"
#include <assert.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Probability {

CalculationController::ContentView::ContentView(Responder * parentResponder, CalculationController * calculationController, Calculation * calculation, Law * law) :
  m_titleView(KDText::FontSize::Small, I18n::Message::ComputeProbability, 0.5f, 0.5f, Palette::GreyDark, Palette::WallScreen),
  m_lawCurveView(law, calculation),
  m_imageTableView(parentResponder, law, calculation, calculationController),
  m_draftTextBuffer{},
  m_calculation(calculation)
{
  for (int i = 0; i < k_maxNumberOfEditableFields; i++) {
    m_calculationCell[i].setParentResponder(parentResponder);
    m_calculationCell[i].textField()->setDelegate(calculationController);
    m_calculationCell[i].textField()->setDraftTextBuffer(m_draftTextBuffer);
  }
}

int CalculationController::ContentView::numberOfSubviews() const {
  return 2*m_calculation->numberOfParameters() + 3;
}

View * CalculationController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 9);
  if (index == 0) {
    return &m_titleView;
  }
  if (index == 1) {
    return &m_lawCurveView;
  }
  if (index == 2) {
    return &m_imageTableView;
  }
  if (index == 3) {
    m_text[0].setMessage(m_calculation->legendForParameterAtIndex(0));
    m_text[0].setAlignment(0.5f, 0.5f);
    return &m_text[0];
  }
  if (index == 5) {
    m_text[1].setMessage(m_calculation->legendForParameterAtIndex(1));
    m_text[1].setAlignment(0.5f, 0.5f);
    return &m_text[1];
  }
  if (index == 7) {
    m_text[2].setMessage(m_calculation->legendForParameterAtIndex(2));
    m_text[2].setAlignment(0.5f, 0.5f);
    return &m_text[2];
  }
  if (index == 4 || index == 6 || index == 8) {
    return &m_calculationCell[(index - 4)/2];
  }
  return nullptr;
}

void CalculationController::ContentView::willDisplayEditableCellAtIndex(int index) {
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  Complex<double>::convertFloatToText(m_calculation->parameterAtIndex(index), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits), Constant::ShortNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
  m_calculationCell[index].textField()->setText(buffer);
}

void CalculationController::ContentView::layoutSubviews() {
  markRectAsDirty(bounds());
  KDCoordinate titleHeight = KDText::charSize(KDText::FontSize::Small).height()+k_titleHeightMargin;
  m_titleView.setFrame(KDRect(0, 0, bounds().width(), titleHeight));
  KDSize charSize = KDText::charSize();
  KDCoordinate xCoordinate = 0;
  m_lawCurveView.setFrame(KDRect(0,  titleHeight+ImageTableView::k_oneCellHeight, bounds().width(), bounds().height() - ImageTableView::k_oneCellHeight-titleHeight));
  KDSize tableSize = m_imageTableView.minimalSizeForOptimalDisplay();
  m_imageTableView.setFrame(KDRect(xCoordinate, titleHeight, tableSize));
  xCoordinate += tableSize.width() + k_textWidthMargin;
  KDCoordinate numberOfCharacters = strlen(I18n::translate(m_calculation->legendForParameterAtIndex(0)));
  m_text[0].setFrame(KDRect(xCoordinate, titleHeight+ImageTableView::k_totalMargin, numberOfCharacters*charSize.width(), ImageCell::k_height));
  xCoordinate += numberOfCharacters*charSize.width() + k_textWidthMargin;
  m_calculationCell[0].setFrame(KDRect(xCoordinate, titleHeight+ImageTableView::k_totalMargin, k_largeTextFieldWidth, ImageCell::k_height));
  xCoordinate += k_largeTextFieldWidth + k_textWidthMargin;
  numberOfCharacters = strlen(I18n::translate(m_calculation->legendForParameterAtIndex(1)));
  m_text[1].setFrame(KDRect(xCoordinate, titleHeight+ImageTableView::k_totalMargin, numberOfCharacters*charSize.width(), ImageCell::k_height));
  xCoordinate += numberOfCharacters*charSize.width() + k_textWidthMargin;
  m_calculationCell[1].setFrame(KDRect(xCoordinate, titleHeight+ImageTableView::k_totalMargin, k_largeTextFieldWidth, ImageCell::k_height));
  xCoordinate += k_largeTextFieldWidth + k_textWidthMargin;
  if (m_calculation->numberOfParameters() > 2) {
    numberOfCharacters = strlen(I18n::translate(m_calculation->legendForParameterAtIndex(2)));;
    m_text[2].setFrame(KDRect(xCoordinate, titleHeight+ImageTableView::k_totalMargin, numberOfCharacters*charSize.width(), ImageCell::k_height));
    xCoordinate += numberOfCharacters*charSize.width() + k_textWidthMargin;
    m_calculationCell[2].setFrame(KDRect(xCoordinate, titleHeight+ImageTableView::k_totalMargin, k_textFieldWidth, ImageCell::k_height));
  }
  for (int k = 0; k < m_calculation->numberOfParameters(); k++) {
    willDisplayEditableCellAtIndex(k);
  }
}

void CalculationController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate titleHeight = KDText::charSize(KDText::FontSize::Small).height()+k_titleHeightMargin;
  ctx->fillRect(KDRect(0,titleHeight, bounds().width(), ImageTableView::k_oneCellWidth), KDColorWhite);
  KDSize charSize = KDText::charSize();
  int numberOfCharacters;
  KDCoordinate xCoordinate = ImageTableView::k_oneCellWidth + k_textWidthMargin;
  KDCoordinate textFieldWidth = k_largeTextFieldWidth;
  for (int i = 0; i < k_maxNumberOfEditableFields; i++) {
    if (m_calculation->numberOfEditableParameters() == i) {
      return;
    }
    if (i == 2) {
      textFieldWidth = k_textFieldWidth;
    }
    numberOfCharacters = strlen(I18n::translate(m_calculation->legendForParameterAtIndex(i)));
    xCoordinate += numberOfCharacters*charSize.width() + k_textWidthMargin;

    ctx->drawRect(KDRect(xCoordinate-ImageTableView::k_outline, titleHeight+ImageTableView::k_margin, textFieldWidth+2*ImageTableView::k_outline, ImageCell::k_height+2*ImageTableView::k_outline), Palette::GreyMiddle);
    xCoordinate += textFieldWidth + k_textWidthMargin;
  }
}

LawCurveView * CalculationController::ContentView::lawCurveView() {
  return &m_lawCurveView;
}

ImageTableView * CalculationController::ContentView::imageTableView() {
  return &m_imageTableView;
}

EditableTextCell * CalculationController::ContentView::calculationCellAtIndex(int index) {
  return &m_calculationCell[index];
}

CalculationController::CalculationController(Responder * parentResponder, Law * law, Calculation * calculation) :
  ViewController(parentResponder),
  m_calculation(calculation),
  m_contentView(this, this, m_calculation, law),
  m_law(law),
  m_highlightedSubviewIndex(1)
{
  assert(law != nullptr);
  assert(calculation != nullptr);
}

View * CalculationController::view() {
  return &m_contentView;
}

const char * CalculationController::title() {
  return m_titleBuffer;
}

void CalculationController::reload() {
  m_contentView.layoutSubviews();
  m_contentView.lawCurveView()->reload();
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

bool CalculationController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::Left && m_highlightedSubviewIndex > 0) || (event == Ion::Events::Right && m_highlightedSubviewIndex < m_calculation->numberOfEditableParameters())) {
    if (m_highlightedSubviewIndex == 0) {
      m_contentView.imageTableView()->select(false);
      m_contentView.imageTableView()->setHighlight(false);
    } else {
      EditableTextCell * calculCell = m_contentView.calculationCellAtIndex(m_highlightedSubviewIndex-1);
      calculCell->setHighlighted(false);
    }
    m_highlightedSubviewIndex = event == Ion::Events::Left ? m_highlightedSubviewIndex - 1 : m_highlightedSubviewIndex + 1;
    if (m_highlightedSubviewIndex > 0) {
      EditableTextCell * newCalculCell = m_contentView.calculationCellAtIndex(m_highlightedSubviewIndex-1);
      newCalculCell->setHighlighted(true);
      app()->setFirstResponder(newCalculCell);
    } else {
      m_contentView.imageTableView()->setHighlight(true);
      app()->setFirstResponder(this);
    }
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Down) && m_highlightedSubviewIndex == 0) {
    m_contentView.imageTableView()->select(true);
    app()->setFirstResponder(m_contentView.imageTableView());
    return true;
  }
  return false;
}

bool CalculationController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return (event == Ion::Events::Right && m_highlightedSubviewIndex < m_calculation->numberOfEditableParameters()) || event == Ion::Events::Left || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool CalculationController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  App * probaApp = (App *)app();
  Context * globalContext = probaApp->container()->globalContext();
  double floatBody = Expression::approximateToScalar<double>(text, *globalContext);
  if (std::isnan(floatBody) || std::isinf(floatBody)) {
    app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  if (m_calculation->type() != Calculation::Type::FiniteIntegral && m_highlightedSubviewIndex == 2) {
    if (floatBody < 0.0) {
      floatBody = 0.0;
    }
    if (floatBody > 1.0) {
      floatBody = 1.0;
    }
  }
  if (!m_law->isContinuous() && (m_highlightedSubviewIndex == 1 || m_calculation->type() == Calculation::Type::FiniteIntegral)) {
    floatBody = std::round(floatBody);
  }
  m_calculation->setParameterAtIndex(floatBody, m_highlightedSubviewIndex-1);
  if (event == Ion::Events::Right || event == Ion::Events::Left) {
    handleEvent(event);
  }
  for (int k = 0; k < m_calculation->numberOfParameters(); k++) {
    m_contentView.willDisplayEditableCellAtIndex(k);
  }
  m_contentView.layoutSubviews();
  return true;
}

bool CalculationController::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::Backspace && !textField->isEditing()) {
    textField->setEditing(true);
    return true;
  }
  return TextFieldDelegate::textFieldDidReceiveEvent(textField, event);
}

void CalculationController::viewWillAppear() {
  reload();
}

void CalculationController::didBecomeFirstResponder() {
  App::Snapshot * snapshot = (App::Snapshot *)app()->snapshot();
  snapshot->setActivePage(App::Snapshot::Page::Calculations);
  updateTitle();
  for (int subviewIndex = 0; subviewIndex < ContentView::k_maxNumberOfEditableFields; subviewIndex++) {
    EditableTextCell * calculCell = m_contentView.calculationCellAtIndex(subviewIndex);
    calculCell->setHighlighted(false);
  }
  if (m_highlightedSubviewIndex > 0) {
    m_contentView.imageTableView()->select(false);
    m_contentView.imageTableView()->setHighlight(false);
    EditableTextCell * calculCell = m_contentView.calculationCellAtIndex(m_highlightedSubviewIndex-1);
    calculCell->setHighlighted(true);
    app()->setFirstResponder(calculCell);
  } else {
    m_contentView.imageTableView()->setHighlight(true);
  }
}

void CalculationController::selectSubview(int subviewIndex) {
  m_highlightedSubviewIndex = subviewIndex;
}

void CalculationController::updateTitle() {
  int currentChar = 0;
  for (int index = 0; index < m_law->numberOfParameter(); index++) {
    m_titleBuffer[currentChar++] = I18n::translate(m_law->parameterNameAtIndex(index))[0];
    strlcpy(m_titleBuffer+currentChar, " = ", 4);
    currentChar += 3;
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
    Complex<double>::convertFloatToText(m_law->parameterValueAtIndex(index), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits), Constant::ShortNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
    strlcpy(m_titleBuffer+currentChar, buffer, strlen(buffer)+1);
    currentChar += strlen(buffer);
    m_titleBuffer[currentChar++] = ' ';
  }
  m_titleBuffer[currentChar-1] = 0;
}

TextFieldDelegateApp * CalculationController::textFieldDelegateApp() {
  return (App *)app();
}

}
