#include "sequence_expression_cell.h"
#include "../app.h"

using namespace Shared;

namespace Sequence {

SequenceExpressionCell::SequenceExpressionCell(Responder * parentResponder) :
  SequenceCell(parentResponder),
  m_expressionView(EvenOddExpressionCell()),
  m_firstInitialConditionView(EvenOddExpressionCell()),
  m_secondInitialConditionView(EvenOddExpressionCell())
{
}

void SequenceExpressionCell::setSequence(Sequence * sequence) {
  SequenceCell::setSequence(sequence);
  bool active = m_sequence->isActive();
  KDColor textColor = active ? KDColorBlack : Palette::GreyDark;
  m_expressionView.setExpression(m_sequence->layout());
  m_expressionView.setTextColor(textColor);
  if (m_numberOfSubCells > 1) {
    m_firstInitialConditionView.setExpression(m_sequence->firstInitialConditionLayout());
    m_firstInitialConditionView.setTextColor(textColor);
  }
  if (m_numberOfSubCells > 2) {
    m_secondInitialConditionView.setExpression(m_sequence->secondInitialConditionLayout());
    m_secondInitialConditionView.setTextColor(textColor);
  }
}

Sequence * SequenceExpressionCell::sequence() {
  return m_sequence;
}

void SequenceExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor separatorColor = m_even ? Palette::WallScreen : KDColorWhite;
  // Color the separators
  ctx->fillRect(KDRect(0, 0, k_separatorThickness, bounds().height()), Palette::GreyBright);
  KDCoordinate cellHeight = (bounds().height()-(m_numberOfSubCells-1)*k_separatorThickness)/m_numberOfSubCells;
  if (m_numberOfSubCells > 1) {
    ctx->fillRect(KDRect(k_separatorThickness, cellHeight, bounds().width() - k_separatorThickness, k_separatorThickness), separatorColor);
  }
  if (m_numberOfSubCells > 2) {
    ctx->fillRect(KDRect(k_separatorThickness, 2*cellHeight+k_separatorThickness, bounds().width() - k_separatorThickness, k_separatorThickness), separatorColor);
  }
}

bool SequenceExpressionCell::handleEvent(Ion::Events::Event event) {
  if (m_selectedSubCell < m_numberOfSubCells - 1 && event == Ion::Events::Down) {
    selectSubCell(m_selectedSubCell+1);
    return true;
  }
  if (m_selectedSubCell > 0 && event == Ion::Events::Up) {
    selectSubCell(m_selectedSubCell-1);
    return true;
  }
  if (event == Ion::Events::OK) {
    editExpression(event);
    return true;
  }
  if (event.hasText() || event == Ion::Events::XNT) {
    editExpression(event);
    return true;
  }
  return false;
}

EvenOddCell * SequenceExpressionCell::viewAtIndex(int index) {
  if (index == 0) {
    return &m_expressionView;
  }
  if (index == 1) {
    return &m_firstInitialConditionView;
  }
  return &m_secondInitialConditionView;
}

void SequenceExpressionCell::editExpression(Ion::Events::Event event) {
  char * initialText = nullptr;
  char initialTextContent[255];
  if (event == Ion::Events::OK) {
    char * text = (char *)m_sequence->text();
    if (m_selectedSubCell == 1) {
      text = (char *)m_sequence->firstInitialConditionText();
    }
    if (m_selectedSubCell == 2) {
      text = (char *)m_sequence->secondInitialConditionText();
    }
    strlcpy(initialTextContent, text, sizeof(initialTextContent));
    initialText = initialTextContent;
  }
  App * myApp = (App *)app();
  InputViewController * inputController = myApp->inputViewController();
  if (m_selectedSubCell == 0) {
    inputController->edit(this, event, this, initialText,
      [](void * context, void * sender){
      SequenceExpressionCell * myCell = (SequenceExpressionCell *) context;
      Sequence * sequence = myCell->sequence();
      InputViewController * myInputViewController = (InputViewController *)sender;
      const char * textBody = myInputViewController->textBody();
      sequence->setContent(textBody);
      myCell->reloadCell();
      SelectableTableView * table = (SelectableTableView *)myCell->parentResponder();
      table->reloadData();
      },
      [](void * context, void * sender){}
    );
  }
  if (m_selectedSubCell == 1) {
    inputController->edit(this, event, this, initialText,
      [](void * context, void * sender){
      SequenceExpressionCell * myCell = (SequenceExpressionCell *) context;
      Sequence * sequence = myCell->sequence();
      InputViewController * myInputViewController = (InputViewController *)sender;
      const char * textBody = myInputViewController->textBody();
      sequence->setFirstInitialConditionContent(textBody);
      myCell->reloadCell();
      SelectableTableView * table = (SelectableTableView *)myCell->parentResponder();
      table->reloadData();
      },
      [](void * context, void * sender){}
    );
  }
  if (m_selectedSubCell == 2) {
    inputController->edit(this, event, this, initialText,
      [](void * context, void * sender){
      SequenceExpressionCell * myCell = (SequenceExpressionCell *) context;
      Sequence * sequence = myCell->sequence();
      InputViewController * myInputViewController = (InputViewController *)sender;
      const char * textBody = myInputViewController->textBody();
      sequence->setSecondInitialConditionContent(textBody);
      myCell->reloadCell();
      SelectableTableView * table = (SelectableTableView *)myCell->parentResponder();
      table->reloadData();
      },
      [](void * context, void * sender){}
    );
  }
}

}
