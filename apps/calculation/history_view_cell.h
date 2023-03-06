#ifndef CALCULATION_HISTORY_VIEW_CELL_H
#define CALCULATION_HISTORY_VIEW_CELL_H

#include <escher/even_odd_cell_with_ellipsis.h>
#include <escher/scrollable_expression_view.h>

#include "../shared/scrollable_multiple_expressions_view.h"
#include "calculation.h"

namespace Calculation {

class HistoryViewCell;

class HistoryViewCellDataSource {
 public:
  enum class SubviewType { None = 0, Input = 1, Output = 2, Ellipsis = 3 };
  HistoryViewCellDataSource() : m_selectedSubviewType(SubviewType::Output) {}
  virtual void setSelectedSubviewType(SubviewType subviewType, bool sameCell,
                                      int previousSelectedX = -1,
                                      int previousSelectedY = -1);
  SubviewType selectedSubviewType() const { return m_selectedSubviewType; }

 private:
  /* This method should belong to a delegate instead of a data source but as
   * both the data source and the delegate will be the same controller, we
   * avoid keeping 2 pointers in HistoryViewCell. */
  // It returns the selected cell at the end of the method
  virtual void historyViewCellDidChangeSelection(HistoryViewCell** cell,
                                                 HistoryViewCell** previousCell,
                                                 int previousSelectedCol,
                                                 int previousSelectedRow,
                                                 SubviewType type,
                                                 SubviewType previousType) = 0;
  SubviewType m_selectedSubviewType;
};

class HistoryViewCell : public Escher::EvenOddCell, public Escher::Responder {
 public:
  constexpr static KDCoordinate k_margin = Escher::Metric::CommonSmallMargin;
  constexpr static KDCoordinate k_inputOutputViewsVerticalMargin = k_margin;
  constexpr static KDCoordinate k_inputViewHorizontalMargin =
      Shared::AbstractScrollableMultipleExpressionsView::k_horizontalMargin;
  constexpr static KDCoordinate k_maxCellHeight = Ion::Display::Height * 2;

  static KDCoordinate Height(Calculation* calculation,
                             Poincare::Context* context, bool expanded);
  HistoryViewCell(Responder* parentResponder = nullptr);
  static bool ViewsCanBeSingleLine(KDCoordinate inputViewWidth,
                                   KDCoordinate outputViewWidth, bool ellipsis);
  void cellDidSelectSubview(
      HistoryViewCellDataSource::SubviewType type,
      HistoryViewCellDataSource::SubviewType previousType =
          HistoryViewCellDataSource::SubviewType::None);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void reloadSubviewHighlight();
  void setDataSource(HistoryViewCellDataSource* dataSource) {
    m_dataSource = dataSource;
  }
  bool displaysSingleLine() const { return m_calculationSingleLine; }
  Responder* responder() override { return this; }
  Poincare::Layout layout() const override;
  KDColor backgroundColor() const override {
    return m_even ? KDColorWhite : Escher::Palette::WallScreen;
  }
  void resetMemoization();
  void setCalculation(Calculation* calculation, bool expanded,
                      Poincare::Context* context,
                      bool canChangeDisplayOutput = false);
  int numberOfSubviews() const override { return 2 + displayedEllipsis(); }
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Shared::ScrollableTwoExpressionsView* outputView() {
    return &m_scrollableOutputView;
  }
  Escher::ScrollableExpressionView* inputView() { return &m_inputView; }
  Calculation::AdditionalInformations additionalInformations() const {
    return m_calculationAdditionInformations;
  }

 private:
  constexpr static KDCoordinate k_resultWidth = 80;
  void updateSubviewsBackgroundAfterChangingState() override;
  void computeSubviewFrames(KDCoordinate frameWidth, KDCoordinate frameHeight,
                            KDRect* ellipsisFrame, KDRect* inputFrame,
                            KDRect* outputFrame);
  void reloadScroll();
  void reloadOutputSelection(
      HistoryViewCellDataSource::SubviewType previousType);
  bool displayedEllipsis() const {
    return isHighlighted() && !m_calculationAdditionInformations.isEmpty();
  }
  uint32_t m_calculationCRC32;
  Calculation::DisplayOutput m_calculationDisplayOutput;
  Calculation::AdditionalInformations m_calculationAdditionInformations;
  Escher::ScrollableExpressionView m_inputView;
  Shared::ScrollableTwoExpressionsView m_scrollableOutputView;
  Escher::EvenOddCellWithEllipsis m_ellipsis;
  HistoryViewCellDataSource* m_dataSource;
  bool m_calculationExpanded;
  bool m_calculationSingleLine;
};

}  // namespace Calculation

#endif
