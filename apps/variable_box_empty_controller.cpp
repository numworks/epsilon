#include "variable_box_empty_controller.h"
#include <poincare/layout_helper.h>
#include <apps/i18n.h>
#include <assert.h>

const KDColor VariableBoxEmptyController::VariableBoxEmptyView::k_backgroundColor;

// VariableBoxEmptyController::VariableBoxEmptyView
void VariableBoxEmptyController::VariableBoxEmptyView::initMessageViews() {
  const int numberOfMessageViews = numberOfMessageTextViews();
  for (int i = 0; i < numberOfMessageViews; i++) {
    MessageTextView * message = messageTextViewAtIndex(i);
    message->setFont(k_font);
    message->setBackgroundColor(k_backgroundColor);
    float verticalAlignment = 0.5f;
    if (i == 0) {
      verticalAlignment = 1.0f;
    } else if (i == numberOfMessageViews - 1) {
      verticalAlignment = 0.0f;
    }
    message->setAlignment(0.5f, verticalAlignment);
  }
}

void VariableBoxEmptyController::VariableBoxEmptyView::setMessages(I18n::Message * message) {
  const int numberOfMessageViews = numberOfMessageTextViews();
  for (int i = 0; i < numberOfMessageViews; i++) {
    messageTextViewAtIndex(i)->setMessage(message[i]);
  }
}

void VariableBoxEmptyController::VariableBoxEmptyView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), k_backgroundColor);
  drawBorderOfRect(ctx, bounds(), Palette::GreyBright);
}

int VariableBoxEmptyController::VariableBoxEmptyView::numberOfSubviews() const {
  return numberOfMessageTextViews() + hasExpressionView();
}

View * VariableBoxEmptyController::VariableBoxEmptyView::subviewAtIndex(int index) {
  if (hasExpressionView()) {
    if (index == k_expressionViewRowIndex) {
      return expressionView();
    }
    return messageTextViewAtIndex(index + (index < k_expressionViewRowIndex ? 0 : -1));
  }
  return messageTextViewAtIndex(index);
}

void VariableBoxEmptyController::VariableBoxEmptyView::layoutSubviews(bool force) {
  const int numberOfMessageViews = numberOfMessageTextViews();
  const bool hasExpression = hasExpressionView();
  KDCoordinate width = bounds().width() - 2 * k_separatorThickness;
  KDCoordinate height = bounds().height() - 2 * k_separatorThickness;
  KDCoordinate textHeight = k_font->glyphSize().height();
  KDCoordinate layoutHeight = hasExpression ? expressionView()->minimalSizeForOptimalDisplay().height() : 0;
  KDCoordinate margin = (height - numberOfMessageViews * textHeight - layoutHeight) / 2;
  if (hasExpression) {
    expressionView()->setFrame(KDRect(
          k_separatorThickness,
          k_separatorThickness + margin + k_expressionViewRowIndex * textHeight,
          width,
          layoutHeight),
        force);
  }
  KDCoordinate currentHeight = k_separatorThickness;
  for (uint8_t i = 0; i < numberOfMessageViews; i++) {
    if (hasExpression && i == k_expressionViewRowIndex) {
      currentHeight += layoutHeight;
    }
    KDCoordinate h = (i == 0 || i == numberOfMessageViews - 1) ? textHeight + margin : textHeight;
    messageTextViewAtIndex(i)->setFrame(KDRect(k_separatorThickness, currentHeight, width, h), force);
    currentHeight += h;
  }
}

// VariableBoxEmptyController

void VariableBoxEmptyController::setMessages(I18n::Message * messages) {
  static_cast<VariableBoxEmptyView *>(view())->setMessages(messages);
}
