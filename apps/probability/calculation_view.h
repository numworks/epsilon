#ifndef PROBABILITY_CALCULATION_VIEW_H
#define PROBABILITY_CALCULATION_VIEW_H

#include <escher.h>
#include "law/law.h"
#include "image_table_view.h"
#include "calculation/calculation.h"
#include "../shared/parameter_text_field_delegate.h"

namespace Probability {

class CalculationController;

class CalculationView : public View, public Responder, public Shared::ParameterTextFieldDelegate {
public:
  CalculationView(Responder * parentResponder, CalculationController * calculationController, Calculation * Calculation, Law * law);
  /* Responder */
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void selectSubview(int subviewIndex);

  /* TextField delegate */
  bool textFieldDidHandleEvent(TextField * textField, Ion::Events::Event event, bool returnValue) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;

  /* View */
  void reload();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void willDisplayEditableCellAtIndex(int index);
  KDSize minimalSizeForOptimalDisplay() const override;
  constexpr static int k_maxNumberOfEditableFields = 3;
private:
  constexpr static KDCoordinate k_minTextFieldWidth = 4*KDText::charSize().width();
  constexpr static KDCoordinate k_maxTextFieldWidth = 10*KDText::charSize().width();
  constexpr static KDCoordinate k_textWidthMargin = 5;
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  void updateCalculationLayout();
  void reloadData();
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  KDCoordinate calculationCellWidth(int index) const;
  ImageTableView m_imageTableView;
  MessageTextView m_text[k_maxNumberOfEditableFields];
  char m_draftTextBuffer[TextField::maxBufferSize()];
  EditableTextCell m_calculationCell[k_maxNumberOfEditableFields];
  Law * m_law;
  Calculation * m_calculation;
  int m_highlightedSubviewIndex;
  CalculationController * m_calculationController;
};

/*class ScrollableCalculationView : public ScrollableView, public ScrollView
}*/
}

#endif
