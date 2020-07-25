#ifndef CALCULATION_HISTORY_VIEW_CELL_H
#define CALCULATION_HISTORY_VIEW_CELL_H

#include <escher.h>
#include "calculation.h"
#include "../shared/scrollable_multiple_expressions_view.h"

namespace Calculation {

class HistoryViewCell;

class HistoryViewCellDataSource {
public:
  enum class SubviewType {
    None = 0,
    Input = 1,
    Output = 2,
    Ellipsis = 3
  };
  HistoryViewCellDataSource() : m_selectedSubviewType(SubviewType::Output) {}
  virtual void setSelectedSubviewType(SubviewType subviewType, bool sameCell, int previousSelectedX = -1, int previousSelectedY = -1);
  SubviewType selectedSubviewType() const { return m_selectedSubviewType; }
private:
  /* This method should belong to a delegate instead of a data source but as
   * both the data source and the delegate will be the same controller, we
   * avoid keeping 2 pointers in HistoryViewCell. */
  // It returns the selected cell at the end of the method
  virtual void historyViewCellDidChangeSelection(HistoryViewCell ** cell, HistoryViewCell ** previousCell, int previousSelectedCellX, int previousSelectedCellY, SubviewType type, SubviewType previousType) = 0;
  SubviewType m_selectedSubviewType;
};

class HistoryViewCell : public ::EvenOddCell, public Responder {
public:
  constexpr static KDCoordinate k_margin = Metric::CommonSmallMargin;
  constexpr static KDCoordinate k_inputOutputViewsVerticalMargin = k_margin;
  constexpr static KDCoordinate k_inputViewHorizontalMargin = Shared::AbstractScrollableMultipleExpressionsView::k_horizontalMargin;
  static KDCoordinate Height(Calculation * calculation, bool expanded);
  HistoryViewCell(Responder * parentResponder = nullptr);
  static bool ViewsCanBeSingleLine(KDCoordinate inputViewWidth, KDCoordinate outputViewWidth);
  void cellDidSelectSubview(HistoryViewCellDataSource::SubviewType type, HistoryViewCellDataSource::SubviewType previousType = HistoryViewCellDataSource::SubviewType::None);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void reloadSubviewHighlight();
  void setDataSource(HistoryViewCellDataSource * dataSource) { m_dataSource = dataSource; }
  bool displaysSingleLine() const {
    return m_calculationSingleLine;
  }
  Responder * responder() override {
    return this;
  }
  Poincare::Layout layout() const override;
  KDColor backgroundColor() const override { return m_even ? KDColorWhite : Palette::WallScreen; }
  void resetMemoization();
  void setCalculation(Calculation * calculation, bool expanded, bool canChangeDisplayOutput = false);
  int numberOfSubviews() const override { return 2 + displayedEllipsis(); }
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Shared::ScrollableTwoExpressionsView * outputView() { return &m_scrollableOutputView; }
  ScrollableExpressionView * inputView() { return &m_inputView; }
  Calculation::AdditionalInformationType additionalInformationType() const { return m_calculationAdditionInformation; }
private:
  constexpr static KDCoordinate k_resultWidth = 80;
  void computeSubviewFrames(KDCoordinate frameWidth, KDCoordinate frameHeight, KDRect * ellipsisFrame, KDRect * inputFrame, KDRect * outputFrame);
  void reloadScroll();
  void reloadOutputSelection(HistoryViewCellDataSource::SubviewType previousType);
  bool displayedEllipsis() const {
    return m_highlighted && m_calculationAdditionInformation != Calculation::AdditionalInformationType::None;
  }
  uint32_t m_calculationCRC32;
  Calculation::DisplayOutput m_calculationDisplayOutput;
  Calculation::AdditionalInformationType m_calculationAdditionInformation;
  ScrollableExpressionView m_inputView;
  Shared::ScrollableTwoExpressionsView m_scrollableOutputView;
  EvenOddCellWithEllipsis m_ellipsis;
  HistoryViewCellDataSource * m_dataSource;
  bool m_calculationExpanded;
  bool m_calculationSingleLine;
};

}

#endif
