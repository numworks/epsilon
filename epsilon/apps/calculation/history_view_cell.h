#ifndef CALCULATION_HISTORY_VIEW_CELL_H
#define CALCULATION_HISTORY_VIEW_CELL_H

#include <apps/calculation/additional_results/additional_results_type.h>
#include <escher/even_odd_cell_with_ellipsis.h>
#include <escher/scrollable_layout_view.h>
#include <escher/scrollable_multiple_layouts_view.h>
#include <omg/troolean.h>

#include "calculation.h"

namespace Calculation {

class HistoryViewCell;

class HistoryViewCellDataSource {
 public:
  enum class SubviewType { None = 0, Input = 1, Output = 2, Ellipsis = 3 };
  HistoryViewCellDataSource() : m_selectedSubviewType(SubviewType::None) {}
  virtual void setSelectedSubviewType(SubviewType subviewType, bool sameCell,
                                      int previousSelectedRow = -1) = 0;
  SubviewType selectedSubviewType() const { return m_selectedSubviewType; }

 protected:
  SubviewType m_selectedSubviewType;
};

class HistoryViewCell : public Escher::EvenOddCell, public Escher::Responder {
 public:
  constexpr static KDCoordinate k_margin = Escher::Metric::CommonSmallMargin;
  constexpr static KDCoordinate k_inputOutputViewsVerticalMargin = k_margin;
  constexpr static KDCoordinate k_inputViewHorizontalMargin =
      Escher::AbstractScrollableMultipleLayoutsView::k_horizontalMargin;
  constexpr static KDCoordinate k_maxCellHeight = Ion::Display::Height * 2;

  static void ComputeCalculationHeights(Calculation* calculation,
                                        Poincare::Context* context);
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
  void setNewCalculation(Calculation* calculation, bool expanded,
                         Poincare::Context* context,
                         bool canChangeDisplayOutput = false);
  int numberOfSubviews() const override { return 2 + isDisplayingEllipsis(); }
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  bool handleEvent(Ion::Events::Event event) override;
  Escher::ScrollableTwoLayoutsView* outputView() {
    return &m_scrollableOutputView;
  }
  Escher::ScrollableLayoutView* inputView() { return &m_inputView; }
  bool hasEllipsis() const { return m_hasEllipsis; }
  KDCoordinate minimalHeightForOptimalDisplay();

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  constexpr static KDCoordinate k_resultWidth = 80;

  KDSize minimalSizeForOptimalDisplay() const override {
    // Not used: we use calculation height memoization
    assert(false);
    return KDSizeZero;
  }
  // This method needs m_calculationDisplayOutput to already be computed
  bool updateExpanded(bool expanded);
  void updateSubviewsBackgroundAfterChangingState() override;
  void computeSubviewFrames(KDCoordinate frameWidth, KDCoordinate frameHeight,
                            KDRect* ellipsisFrame, KDRect* inputFrame,
                            KDRect* outputFrame);
  void reloadScroll();
  void reloadOutputSelection(
      HistoryViewCellDataSource::SubviewType previousType);
  bool isDisplayingEllipsis() const { return isHighlighted() && hasEllipsis(); }
  uint32_t m_calculationCRC32;
  Calculation::DisplayOutput m_calculationDisplayOutput;
  bool m_hasEllipsis;
  Escher::ScrollableLayoutView m_inputView;
  Escher::ScrollableTwoLayoutsView m_scrollableOutputView;
  Escher::EvenOddCellWithEllipsis m_ellipsis;
  HistoryViewCellDataSource* m_dataSource;
  OMG::Troolean m_calculationExpanded;
  bool m_calculationSingleLine;
};

}  // namespace Calculation

#endif
