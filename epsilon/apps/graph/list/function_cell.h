#pragma once

#include <apps/i18n.h>
#include <apps/shared/with_expression_cell.h>
#include <escher/ellipsis_view.h>
#include <escher/even_odd_cell.h>
#include <escher/message_text_view.h>
#include <escher/metric.h>
#include <escher/theme/components.h>

namespace Graph {

// TODO: factorize with AbstractSequenceCell which is very similar
class AbstractFunctionCell : public Escher::EvenOddCell {
 public:
  AbstractFunctionCell();
  virtual Escher::HighlightCell* mainCell() = 0;
  const Escher::HighlightCell* mainCell() const {
    return const_cast<Escher::HighlightCell*>(
        const_cast<AbstractFunctionCell*>(this)->mainCell());
  }

  // View
  KDSize minimalSizeForOptimalDisplay() const override;

  // EvenOddCell
  void drawRect(KDContext* ctx, KDRect rect) const override;

  // - Message Text View
  void setMessage(I18n::Message message) {
    m_messageTextView.setMessage(message);
  }
  void setHideMessage(bool hide) { m_hideMessage = hide; }

  // - Color indicator
  void setColor(Escher::ThemeComponent color) { m_functionColor = color; }

  /* Background color logic is simpler than in EvenOddCell (no invisible cells)
   * and the highlight logic is resolved separately in the drawing function,
   * depending on which subview is selected (expression or ellipsis) */
  Escher::ThemeComponent backgroundColor() const override {
    return baseColor();
  }

  void setHighlighted(bool highlight) override;

 protected:
  // View
  bool displayFunctionType() const;
  virtual bool displayEllipsis() const = 0;
  int numberOfSubviews() const override { return 2 + displayFunctionType(); }
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  KDCoordinate messageTextHeight() const;
  KDCoordinate ellipsisViewWidth() const {
    return displayEllipsis() ? Escher::Metric::EllipsisCellWidth : 0;
  }
  /* Note: in the base version expressionIsHighlighted(),
   * ellipsisIsHighlighted() and isHighlighted() mean the same thing, but it is
   * not the case in the FunctionCell override. */
  virtual bool expressionIsHighlighted() const { return isHighlighted(); }
  virtual bool ellipsisIsHighlighted() const { return isHighlighted(); }

  constexpr static KDCoordinate k_colorIndicatorThickness =
      Escher::Metric::VerticalColorIndicatorThickness;
  constexpr static KDCoordinate k_margin =
      Escher::Metric::CellHorizontalElementMargin;
  constexpr static KDCoordinate k_messageMargin =
      Escher::Metric::CellVerticalElementMargin;
  Escher::MessageTextView m_messageTextView;
  Escher::EllipsisView m_ellipsisView;
  Escher::ThemeComponent m_functionColor;
  bool m_hideMessage;
};

template <typename T>
class TemplatedFunctionCell : public AbstractFunctionCell, public T {
 public:
  TemplatedFunctionCell() : AbstractFunctionCell(), T() {}

 protected:
  Escher::HighlightCell* mainCell() override { return T::expressionCell(); }
};

class FunctionCell
    : public TemplatedFunctionCell<Shared::WithNonEditableExpressionCell> {
 public:
  FunctionCell()
      : TemplatedFunctionCell<Shared::WithNonEditableExpressionCell>(),
        m_parameterSelected(false) {}
  // - Ellipsis View
  void setParameterSelected(bool selected);

  bool expressionIsHighlighted() const override {
    return AbstractFunctionCell::expressionIsHighlighted() &&
           !m_parameterSelected;
  }
  bool ellipsisIsHighlighted() const override {
    return AbstractFunctionCell::ellipsisIsHighlighted() && m_parameterSelected;
  }

 private:
  void updateSubviewsBackgroundAfterChangingState() override;
  bool displayEllipsis() const override { return true; }

  bool m_parameterSelected;
};

}  // namespace Graph
