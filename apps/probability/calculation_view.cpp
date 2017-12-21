#include "calculation_view.h"
#include "app.h"
#include "../apps_container.h"
#include "calculation_controller.h"
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Probability {

CalculationView::CalculationView(Responder * parentResponder, CalculationController * calculationController, Calculation * calculation, Law * law) :
  Responder(parentResponder),
  m_imageTableView(this, law, calculation, calculationController),
  m_draftTextBuffer{},
  m_law(law),
  m_calculation(calculation),
  m_highlightedSubviewIndex(1),
  m_calculationController(calculationController)
{
  for (int i = 0; i < k_maxNumberOfEditableFields; i++) {
    m_calculationCell[i].setParentResponder(this);
    m_calculationCell[i].textField()->setDelegate(this);
    m_calculationCell[i].textField()->setDraftTextBuffer(m_draftTextBuffer);
    m_text[i].setAlignment(0.5f, 0.5f);
  }
}

void CalculationView::didBecomeFirstResponder() {
  if (m_highlightedSubviewIndex > 0) {
    app()->setFirstResponder(&m_calculationCell[m_highlightedSubviewIndex-1]);
  } else {
    app()->setFirstResponder(&m_imageTableView);
  }
}

bool CalculationView::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::Left && m_highlightedSubviewIndex > 0) || (event == Ion::Events::Right && m_highlightedSubviewIndex < m_calculation->numberOfEditableParameters())) {
    if (m_highlightedSubviewIndex == 0) {
      m_imageTableView.select(false);
    } else {
      m_calculationCell[m_highlightedSubviewIndex-1].setHighlighted(false);
    }
    m_highlightedSubviewIndex = event == Ion::Events::Left ? m_highlightedSubviewIndex - 1 : m_highlightedSubviewIndex + 1;
    if (m_highlightedSubviewIndex > 0) {
      m_calculationCell[m_highlightedSubviewIndex-1].setHighlighted(true);
      app()->setFirstResponder(&m_calculationCell[m_highlightedSubviewIndex-1]);
    } else {
      app()->setFirstResponder(&m_imageTableView);
    }
    return true;
  }
  return false;
}

void CalculationView::selectSubview(int subviewIndex) {
  m_highlightedSubviewIndex = subviewIndex;
}

bool CalculationView::textFieldDidHandleEvent(::TextField * textField, Ion::Events::Event event, bool returnValue, bool textHasChanged) {
  if (returnValue && textHasChanged) {
    updateCalculationLayoutFromIndex(m_highlightedSubviewIndex-1);
  }
  return returnValue;
}

bool CalculationView::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event)
      || (event == Ion::Events::Right && textField->cursorLocation() == textField->draftTextLength() && m_highlightedSubviewIndex < m_calculation->numberOfEditableParameters())
      || (event == Ion::Events::Left && textField->cursorLocation() == 0);
}

bool CalculationView::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
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
  reloadData();
  updateCalculationLayoutFromIndex(0);
  m_calculationController->reloadLawCurveView();
  return true;
}

void CalculationView::reload() {
  markRectAsDirty(bounds());
  layoutSubviews();
}

void CalculationView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0,0, bounds().width(), ImageTableView::k_oneCellHeight), KDColorWhite);
  KDSize charSize = KDText::charSize();
  int numberOfCharacters;
  KDCoordinate xCoordinate = ImageTableView::k_oneCellWidth + k_textWidthMargin;
  for (int i = 0; i < k_maxNumberOfEditableFields; i++) {
    if (m_calculation->numberOfEditableParameters() == i) {
      return;
    }
    numberOfCharacters = strlen(I18n::translate(m_calculation->legendForParameterAtIndex(i)));
    xCoordinate += numberOfCharacters*charSize.width() + k_textWidthMargin;

    ctx->strokeRect(KDRect(xCoordinate-ImageTableView::k_outline, ImageTableView::k_margin, calculationCellWidth(i)+2*ImageTableView::k_outline, ImageCell::k_height+2*ImageTableView::k_outline), Palette::GreyMiddle);
    xCoordinate += calculationCellWidth(i) + k_textWidthMargin;
  }
}

void CalculationView::willDisplayEditableCellAtIndex(int index) {
  m_calculationCell[index].setHighlighted(index == m_highlightedSubviewIndex-1);
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  Complex<double>::convertFloatToText(m_calculation->parameterAtIndex(index), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, Expression::FloatDisplayMode::Decimal);
  m_calculationCell[index].textField()->setText(buffer);
}

KDSize CalculationView::minimalSizeForOptimalDisplay() const {
  //xCoordinate += tableSize.width() + k_textWidthMargin;
  return KDSize(0, ImageTableView::k_oneCellHeight);
}

TextFieldDelegateApp * CalculationView::textFieldDelegateApp() {
  return (App *)app();
}

void CalculationView::updateCalculationLayoutFromIndex(int index) {
  KDSize charSize = KDText::charSize();
  KDCoordinate numberOfCharacters = strlen(I18n::translate(m_calculation->legendForParameterAtIndex(0)));
  KDCoordinate xCoordinate = m_imageTableView.minimalSizeForOptimalDisplay().width() + 2*k_textWidthMargin+numberOfCharacters*charSize.width();
  KDCoordinate calculationWidth = calculationCellWidth(0);
  if (index == 0) {
    markRectAsDirty(KDRect(xCoordinate, ImageTableView::k_totalMargin-1, bounds().width() - xCoordinate, ImageCell::k_height+2));
    m_calculationCell[0].setFrame(KDRect(xCoordinate, ImageTableView::k_totalMargin, calculationWidth, ImageCell::k_height));
  }

  xCoordinate += calculationWidth + k_textWidthMargin;
  numberOfCharacters = strlen(I18n::translate(m_calculation->legendForParameterAtIndex(1)));
  if (index == 0) {
  m_text[1].setFrame(KDRect(xCoordinate, ImageTableView::k_totalMargin, numberOfCharacters*charSize.width(), ImageCell::k_height));
  }
  xCoordinate += numberOfCharacters*charSize.width() + k_textWidthMargin;
  calculationWidth = calculationCellWidth(1);
  if (index <= 1) {
    markRectAsDirty(KDRect(xCoordinate, ImageTableView::k_totalMargin-1, bounds().width() - xCoordinate, ImageCell::k_height+2));
    m_calculationCell[1].setFrame(KDRect(xCoordinate, ImageTableView::k_totalMargin, calculationWidth, ImageCell::k_height));
  }
  xCoordinate += calculationWidth + k_textWidthMargin;
  if (m_calculation->numberOfParameters() > 2) {
    numberOfCharacters = strlen(I18n::translate(m_calculation->legendForParameterAtIndex(2)));;
    m_text[2].setFrame(KDRect(xCoordinate, ImageTableView::k_totalMargin, numberOfCharacters*charSize.width(), ImageCell::k_height));
    xCoordinate += numberOfCharacters*charSize.width() + k_textWidthMargin;
    calculationWidth = calculationCellWidth(2);
    markRectAsDirty(KDRect(xCoordinate, ImageTableView::k_totalMargin-1, bounds().width() - xCoordinate, ImageCell::k_height+2));
    m_calculationCell[2].setFrame(KDRect(xCoordinate, ImageTableView::k_totalMargin, calculationWidth, ImageCell::k_height));
  }
}

void CalculationView::reloadData() {
  for (int k = 0; k < m_calculation->numberOfParameters(); k++) {
    willDisplayEditableCellAtIndex(k);
  }
}

int CalculationView::numberOfSubviews() const {
  return 2*m_calculation->numberOfParameters() + 1;
}

View * CalculationView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 7);
  if (index == 0) {
    return &m_imageTableView;
  }
  if (index == 1) {
    m_text[0].setMessage(m_calculation->legendForParameterAtIndex(0));
    m_text[0].setAlignment(0.5f, 0.5f);
    return &m_text[0];
  }
  if (index == 3) {
    m_text[1].setMessage(m_calculation->legendForParameterAtIndex(1));
    m_text[1].setAlignment(0.5f, 0.5f);
    return &m_text[1];
  }
  if (index == 5) {
    m_text[2].setMessage(m_calculation->legendForParameterAtIndex(2));
    m_text[2].setAlignment(0.5f, 0.5f);
    return &m_text[2];
  }
  if (index == 2 || index == 4 || index == 6) {
    return &m_calculationCell[(index - 2)/2];
  }
  return nullptr;
}

void CalculationView::layoutSubviews() {
  // Reload values in textFields
  reloadData();
  // Reload messages
  m_text[0].setMessage(m_calculation->legendForParameterAtIndex(0));
  m_text[1].setMessage(m_calculation->legendForParameterAtIndex(1));
  if (m_calculation->numberOfParameters() == 3) {
    m_text[2].setMessage(m_calculation->legendForParameterAtIndex(2));
  }
  // Layout
  KDSize charSize = KDText::charSize();
  KDCoordinate xCoordinate = 0;
  KDSize tableSize = m_imageTableView.minimalSizeForOptimalDisplay();
  m_imageTableView.setFrame(KDRect(xCoordinate, 0, tableSize));
  xCoordinate += tableSize.width() + k_textWidthMargin;
  KDCoordinate numberOfCharacters = strlen(I18n::translate(m_calculation->legendForParameterAtIndex(0)));
  m_text[0].setFrame(KDRect(xCoordinate, ImageTableView::k_totalMargin, numberOfCharacters*charSize.width(), ImageCell::k_height));

  updateCalculationLayoutFromIndex(0);
}

KDCoordinate CalculationView::calculationCellWidth(int index) const {
  KDCoordinate calculationCellWidth = m_calculationCell[index].minimalSizeForOptimalDisplay().width();
  return min(k_maxTextFieldWidth, max(k_minTextFieldWidth, calculationCellWidth));
}

}
