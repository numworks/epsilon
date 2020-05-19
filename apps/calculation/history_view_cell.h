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
    None,
    Input,
    Output,
    Ellipsis
  };
  HistoryViewCellDataSource();
  void setSelectedSubviewType(SubviewType subviewType, bool sameCell, int previousSelectedX = -1, int previousSelectedY = -1);
  SubviewType selectedSubviewType() { return m_selectedSubviewType; }
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
  HistoryViewCell(Responder * parentResponder = nullptr);
  static bool CanBeSingleLine(KDCoordinate inputWidth, KDCoordinate outputWidth);
  void cellDidSelectSubview(HistoryViewCellDataSource::SubviewType type, HistoryViewCellDataSource::SubviewType previousType = HistoryViewCellDataSource::SubviewType::None);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void reloadSubviewHighlight();
  void setDataSource(HistoryViewCellDataSource * dataSource) { m_dataSource = dataSource; }
  Responder * responder() override {
    return this;
  }
  Poincare::Layout layout() const override;
  KDColor backgroundColor() const override;
  void resetMemoization();
  void setCalculation(Calculation * calculation, bool expanded);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Shared::ScrollableTwoExpressionsView * outputView();
  Calculation::AdditionalInformationType additionalInformationType() const { return m_calculationAdditionInformation; }
private:
  constexpr static KDCoordinate k_resultWidth = 80;
  void reloadScroll();
  void reloadOutputSelection(HistoryViewCellDataSource::SubviewType previousType);
  bool displayedEllipsis() const;
  uint32_t m_calculationCRC32;
  Calculation::DisplayOutput m_calculationDisplayOutput;
  Calculation::AdditionalInformationType m_calculationAdditionInformation;
  bool m_calculationExpanded;
  ScrollableExpressionView m_inputView;
  Shared::ScrollableTwoExpressionsView m_scrollableOutputView;
  EvenOddCellWithEllipsis m_ellipsis;
  HistoryViewCellDataSource * m_dataSource;
};

}

#endif
