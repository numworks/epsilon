#ifndef SEQUENCE_GRAPH_VIEW_H
#define SEQUENCE_GRAPH_VIEW_H

#include <apps/shared/function_graph_view.h>
#include <apps/shared/sequence_store.h>

namespace Sequence {

class GraphView : public Shared::FunctionGraphView {
 public:
  GraphView(Shared::SequenceStore* sequenceStore,
            Shared::InteractiveCurveViewRange* graphRange,
            Shared::CurveViewCursor* cursor, Shared::BannerView* bannerView,
            Shared::CursorView* cursorView);

  // FunctionGraphView
  int numberOfDrawnRecords() const override {
    return m_sequenceStore->numberOfActiveFunctions();
  }
  void drawRecord(Ion::Storage::Record record, int index, KDContext* ctx,
                  KDRect rect, bool firstDrawnRecord) const override;
  int nextDotIndex(Shared::Sequence* sequence, int currentIndex,
                   OMG::HorizontalDirection direction = OMG::Direction::Right(),
                   int scrollSpeed = 1) const;
  void tidyModel(int i, Poincare::PoolObject* treePoolCursor) const override {
    m_sequenceStore->modelForRecord(m_sequenceStore->activeRecordAtIndex(i))
        ->tidyDownstreamPoolFrom(treePoolCursor);
  }
  int selectedRecordIndex() const override {
    return m_sequenceStore->indexOfRecordAmongActiveRecords(m_selectedRecord);
  }
  Shared::FunctionStore* functionStore() const override {
    return m_sequenceStore;
  }

 private:
  Shared::SequenceStore* m_sequenceStore;
};

}  // namespace Sequence

#endif
